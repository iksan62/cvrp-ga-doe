import os
import warnings
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import shapiro, kruskal
import statsmodels.api as sm
from statsmodels.formula.api import ols

def analyze_ga_parameters_and_save_results(file_path, output_dir):
    warnings.filterwarnings("ignore", category=UserWarning)
    
    if not os.path.exists(output_dir):
        try:
            os.makedirs(output_dir)
            print(f"Directory '{output_dir}' successfully created.")
        except OSError as e:
            print(f"ERROR: Failed to create directory '{output_dir}'. Error: {e}")
            return

    with open(os.path.join(output_dir, 'analysis_summary.txt'), 'w') as f:
        def log(message):
            print(message)
            f.write(message + '\n')

        try:
            df_full_history = pd.read_csv(file_path)
            log(f"Successfully loaded file: {os.path.abspath(file_path)}")
        except FileNotFoundError:
            log(f"ERROR: File not found at '{file_path}'. Please ensure the file exists.")
            return

        df_full_history.columns = df_full_history.columns.str.strip()
        run_columns = [col for col in df_full_history.columns if 'run ke-' in col]
        config_params_id = ['methods', 'cross_rate', 'mut_rate', 'breed_rate', 'num_generations']
        
        log("Analysis started...")
        log(f"All output files will be saved in: {os.path.abspath(output_dir)}")
        log("\nStep 1: Finding the best (minimum) fitness value from each run's history.")
        
        df_best_fitness = df_full_history.groupby(config_params_id)[run_columns].min().reset_index()
        log("Step 1: Completed.")

        id_vars = [col for col in df_best_fitness.columns if col not in run_columns]
        df_long = pd.melt(df_best_fitness, id_vars=id_vars, value_vars=run_columns, var_name='run_id', value_name='fitness')
        
        log("\n" + "="*60)
        log("PART 1: DETERMINING OPTIMAL PARAMETERS PER METHOD")
        log("="*60)
        
        summary_per_method = df_long.groupby(config_params_id)['fitness'].agg(['mean', 'std']).reset_index()
        
        log("\nSaving complete summary table (mean and std) for all configurations...")
        summary_path = os.path.join(output_dir, 'Full_summary_mean_std_all_configs.csv')
        summary_per_method.to_csv(summary_path, index=False)
        log(f"--> Complete summary table saved to: {summary_path}")

        optimal_configs = summary_per_method.loc[summary_per_method.groupby('methods')['mean'].idxmin()]
        log("\nOptimal Configurations Found for Each Method:")
        log(optimal_configs.to_string(index=False))
        
        optimal_configs_path = os.path.join(output_dir, 'Part1_optimal_configs_per_method.csv')
        optimal_configs.to_csv(optimal_configs_path, index=False)
        log(f"\n--> The above table saved to: {optimal_configs_path}")

        log("\n--- Statistical Validation for Each Method ---")
        for method in sorted(df_long['methods'].unique()):
            log(f"\n-> Method: {method}")
            method_data = df_long[df_long['methods'] == method].copy()
            
            stat, p_shapiro = shapiro(method_data['fitness'])
            
            log(f"   Normality Test (Shapiro-Wilk): p-value = {p_shapiro:.4f}")
            if p_shapiro > 0.05:
                log("   Data appears normal. Using ANOVA.")
                model = ols('fitness ~ C(cross_rate) + C(mut_rate) + C(num_generations)', data=method_data).fit()
                anova_table = sm.stats.anova_lm(model, typ=2)
                log("   ANOVA Results (p-values < 0.05 indicate significant factors):")
                log(anova_table.to_string())
                
                anova_path = os.path.join(output_dir, f'Part1_validation_{method}_ANOVA.csv')
                anova_table.to_csv(anova_path)
                log(f"   --> ANOVA table saved to: {anova_path}")
            else:
                log("   Data is not normal. Using Kruskal-Wallis.")
                kruskal_results = []
                for param in ['cross_rate', 'mut_rate', 'num_generations']:
                    groups = [group['fitness'].values for name, group in method_data.groupby(param)]
                    if len(groups) > 1:
                        stat_kw, p_kw = kruskal(*groups)
                        significance = 'Significant' if p_kw < 0.05 else 'Not Significant'
                        kruskal_results.append({'Factor': param, 'p-value': p_kw, 'Conclusion': significance})
                
                kruskal_df = pd.DataFrame(kruskal_results)
                log(kruskal_df.to_string(index=False))
                
                kruskal_path = os.path.join(output_dir, f'Part1_validation_{method}_Kruskal.csv')
                kruskal_df.to_csv(kruskal_path, index=False)
                log(f"   --> Kruskal-Wallis table saved to: {kruskal_path}")

        log("\n\n" + "="*60)
        log("PART 2: DETERMINING UNIFORM OPTIMAL PARAMETERS FOR ALL METHODS")
        log("="*60)
        
        config_params = ['cross_rate', 'mut_rate', 'breed_rate', 'num_generations']
        uniform_summary = summary_per_method.groupby(config_params)['mean'].mean().reset_index()
        uniform_summary.rename(columns={'mean': 'overall_mean_fitness'}, inplace=True)
        best_uniform_config_series = uniform_summary.loc[uniform_summary['overall_mean_fitness'].idxmin()]
        
        best_uniform_config_df = best_uniform_config_series.to_frame().T
        log("\nBest Uniform Optimal Configuration for All Methods:")
        log(best_uniform_config_df.to_string(index=False))
        
        uniform_path = os.path.join(output_dir, 'Part2_optimal_uniform_config.csv')
        best_uniform_config_df.to_csv(uniform_path, index=False)
        log(f"\n--> The above table saved to: {uniform_path}")
        
        log("\n--- Validation for Uniform Configuration ---")
        log("Kruskal-Wallis test to compare method performance ON this optimal configuration.")
    
        query = ' & '.join([f"`{key}` == {value}" for key, value in best_uniform_config_series.to_dict().items() if key in config_params])
        validation_data = df_long.query(query)
        
        method_groups = [group['fitness'].values for name, group in validation_data.groupby('methods')]
        
        validation_result = {}
        if len(method_groups) > 1:
            stat_kw_uniform, p_kw_uniform = kruskal(*method_groups)
            conclusion = "Supports the use of this parameter set as a fair uniform configuration." if p_kw_uniform > 0.05 else "Indicates some methods perform significantly differently with this parameter set."
            log(f"\nKruskal-Wallis Test Results: p-value = {p_kw_uniform:.4f}")
            log(f"Conclusion: {conclusion}")
            validation_result = {'test': 'Kruskal-Wallis', 'p-value': p_kw_uniform, 'conclusion': conclusion}
        else:
            log("\nInsufficient data/methods to perform validation test.")
            validation_result = {'test': 'Kruskal-Wallis', 'p-value': None, 'conclusion': 'Insufficient data'}
            
        validation_df = pd.DataFrame([validation_result])
        validation_path = os.path.join(output_dir, 'Part2_uniform_config_validation.csv')
        validation_df.to_csv(validation_path, index=False)
        log(f"--> Validation results saved to: {validation_path}")

        log("\n\n" + "="*60)
        log("PART 3: GENERATING MAIN EFFECTS TABLE AND PLOT")
        log("="*60)

        param_cols = ['cross_rate', 'mut_rate', 'breed_rate', 'num_generations']
        response_col = 'mean'

        def main_effects_table(df, param_cols, response_col='mean', by=None):
            tables = []
            for col in param_cols:
                group_cols = [col] + ([by] if by else [])
                agg_df = df.groupby(group_cols, dropna=False)[response_col].agg(
                    mean='mean',
                    std='std',
                    count='count',
                    median='median',
                    q1=lambda x: x.quantile(0.25),
                    q3=lambda x: x.quantile(0.75)
                ).reset_index()

                agg_df['se'] = agg_df['std'] / np.sqrt(agg_df['count'].clip(lower=1))
                agg_df['ci95_lower'] = agg_df['mean'] - 1.96 * agg_df['se']
                agg_df['ci95_upper'] = agg_df['mean'] + 1.96 * agg_df['se']
                agg_df['iqr'] = agg_df['q3'] - agg_df['q1']

                agg_df.insert(0, 'Parameter', col)

                if np.issubdtype(df[col].dtype, np.number):
                    agg_df = agg_df.sort_values(by=[col] + ([by] if by else []))
                else:
                    agg_df = agg_df.sort_values(by=[col] + ([by] if by else []), key=lambda s: s.astype(str))

                tables.append(agg_df)

            result = pd.concat(tables, ignore_index=True)

            sort_keys = ['Parameter'] + [c for c in result.columns if c in param_cols] + ([by] if by else [])
            result = result.sort_values(sort_keys)

            num_cols = ['mean', 'std', 'median', 'q1', 'q3', 'iqr', 'se', 'ci95_lower', 'ci95_upper']
            for c in num_cols:
                if c in result.columns:
                    result[c] = result[c].astype(float)

            return result

        table_overall = main_effects_table(summary_per_method, param_cols, response_col=response_col)

        csv_path = os.path.join(output_dir, 'Table_MainEffects_Overall.csv')
        tex_path = os.path.join(output_dir, 'Table_MainEffects_Overall.tex')

        table_overall.to_csv(csv_path, index=False)
        log(f"--> Main effects table (overall) saved: {csv_path}")

        try:
            with open(tex_path, 'w', encoding='utf-8') as f_tex:
                f_tex.write(table_overall.to_latex(index=False, float_format="%.3f"))
            log(f"--> LaTeX table saved: {tex_path}")
        except Exception as e:
            log(f"--> LaTeX export skipped: {e}")

        params = {
            'cross_rate': 'Crossover Rate ($P_c$)',
            'mut_rate': 'Mutation Rate ($P_m$)',
            'breed_rate': 'Breeder Rate ($P_b$)',
            'num_generations': 'Generations ($G$)'
        }

        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Main Effects Plot for Mean Fitness (Lower is Better)', fontsize=16, y=0.98)
        axes = axes.flatten()

        for i, (col, label) in enumerate(params.items()):
            ax = axes[i]
            effect_data = summary_per_method.groupby(col)[response_col].mean().sort_index()
            x_values = effect_data.index.astype(str)
            y_values = effect_data.values

            ax.plot(x_values, y_values, marker='o', linestyle='-', linewidth=2.5, color='#2c3e50')
            ax.set_title(label, fontsize=12, fontweight='bold')
            ax.grid(True, linestyle='--', alpha=0.6)
            if i % 2 == 0:
                ax.set_ylabel('Mean Fitness', fontsize=10)
            for x, y in zip(x_values, y_values):
                ax.text(x, y + (y * 0.005), f'{y:,.0f}', ha='center', va='bottom', fontsize=9, color='blue')

        plt.tight_layout(rect=[0, 0.03, 1, 0.95])

        fig_path_svg = os.path.join(output_dir, 'Figure2_MainEffects.svg')
        plt.savefig(fig_path_svg, dpi=300)
        plt.show()

        log(f"--> Figure 2 successfully created and saved as: {fig_path_svg}")
        
        log("\n\n" + "="*60)
        log("Analysis Completed.")

if __name__ == '__main__':
    input_file = os.path.join('D:', 'dat2.csv')
    output_directory = os.path.join('D:', 'phase_2')
    analyze_ga_parameters_and_save_results(file_path=input_file, output_dir=output_directory)