import pandas as pd
import numpy as np
import os
import sys

def preprocess_raw_data(file_path):
    try:
        df = pd.read_csv(file_path)
    except FileNotFoundError:
        print(f"Error: File not found at '{file_path}'. Please check your file path.")
        return None
    
    id_vars = ['benchmark', 'methods', 'pop_size', 'generations']
    value_vars = [col for col in df.columns if 'run ke-' in col]
    
    df_long = pd.melt(df, id_vars=id_vars, value_vars=value_vars,
                      var_name='run_id', value_name='fitness')
    df_long['methods'] = df_long['methods'].str.upper()
    df_long['run_id'] = df_long['run_id'].str.replace('run ke-', '').astype(int)
    
    print("Searching for the best (minimum) fitness value from each run...")
    
    run_identifiers = ['benchmark', 'methods', 'pop_size', 'run_id']
    best_fitness_per_run = df_long.loc[df_long.groupby(run_identifiers)['fitness'].idxmin()]
    
    final_df = best_fitness_per_run.drop(columns=['generations', 'run_id'])
    
    print("'Best-so-far' data per run has been successfully prepared.")
    return final_df

def create_summary_table(df_long, benchmarks):
    print("Creating fitness summary table for Phase 1...")
    fitness_summary = df_long.groupby(['benchmark', 'methods', 'pop_size'])['fitness'].agg(['mean', 'std']).reset_index()
    
    best_known = {
        'X-n106-k14': 26362,
        'X-n502-k39': 69223,
        'X-n1001-k43': 72355
    }
    fitness_summary['normalized_mean'] = fitness_summary.apply(
        lambda row: row['mean'] / best_known.get(row['benchmark'], np.nan), axis=1)
    
    pivoted_df = fitness_summary.pivot_table(
        index=['methods', 'pop_size'],
        columns='benchmark',
        values=['mean', 'std', 'normalized_mean']
    ).reset_index()
    pivoted_df.columns = [f'{col[1]}_{col[0]}' if col[1] != '' else col[0] for col in pivoted_df.columns]
    
    final_column_order = ['methods', 'pop_size']
    for b in benchmarks:
        final_column_order.extend([f'{b}_mean', f'{b}_std', f'{b}_normalized_mean'])
    
    summary_table = pivoted_df[[col for col in final_column_order if col in pivoted_df.columns]]
    
    summary_table['avg_normalized_fitness'] = (
        summary_table.get('X-n106-k14_normalized_mean', 0) * 106 +
        summary_table.get('X-n502-k39_normalized_mean', 0) * 502 +
        summary_table.get('X-n1001-k43_normalized_mean', 0) * 1001
    ) / (106 + 502 + 1001)
    
    pop_size_summary = summary_table.groupby('pop_size')['avg_normalized_fitness'].mean().reset_index()
    best_pop_size = pop_size_summary.loc[pop_size_summary['avg_normalized_fitness'].idxmin(), 'pop_size']
    print(f"Optimal uniform population size: {int(best_pop_size)}")
    
    optimal_params = []
    for method in summary_table['methods'].unique():
        method_data = summary_table[summary_table['methods'] == method].copy()
        if not method_data.empty:
            best_row = method_data.loc[method_data['avg_normalized_fitness'].idxmin()]
            optimal_params.append({
                'Method': method,
                'Optimal_Pop_Size': int(best_row['pop_size']),
                'Avg_Normalized_Fitness': best_row['avg_normalized_fitness'],
                'X-n106-k14_Mean': best_row.get('X-n106-k14_mean'),
                'X-n502-k39_Mean': best_row.get('X-n502-k39_mean'),
                'X-n1001-k43_Mean': best_row.get('X-n1001-k43_mean')
            })
    optimal_params_df = pd.DataFrame(optimal_params)
    
    return summary_table, int(best_pop_size), optimal_params_df, pop_size_summary

if __name__ == "__main__":
    # --- PENGATURAN DIREKTORI OUTPUT ---
    output_dir = os.path.join('D:\\', 'phase_1')
    
    # Buat folder jika belum ada
    os.makedirs(output_dir, exist_ok=True)
    
    # Path file input (tetap di D:\dat.csv)
    phase1_file_path = os.path.join('D:\\', 'dat.csv')
    
    # Path file output (sekarang semuanya masuk ke folder phase_1)
    csv_summary_phase1_path = os.path.join(output_dir, 'phase1_summary_best_so_far.csv')
    csv_optimal_params_path = os.path.join(output_dir, 'phase1_optimal_pop_size_best_so_far.csv')
    txt_output_path = os.path.join(output_dir, 'phase1_analysis_best_so_far.txt')
    csv_comparison_path = os.path.join(output_dir, 'pop_size_comparison.csv')
    
    df_best_per_run = preprocess_raw_data(phase1_file_path)
    
    if df_best_per_run is not None and not df_best_per_run.empty:
        benchmarks_list = ['X-n106-k14', 'X-n502-k39', 'X-n1001-k43']
        summary_table, optimal_pop_size, optimal_params_df, pop_size_summary_df = create_summary_table(
            df_best_per_run, benchmarks=benchmarks_list
        )
        
        original_stdout = sys.stdout
        try:
            with open(txt_output_path, 'w', encoding='utf-8') as f:
                sys.stdout = f
                print(f"PHASE 1 ANALYSIS (BEST-SO-FAR) - Executed on {pd.Timestamp.now()}")
                print("\n" + "="*50 + "\nSummary Table\n" + "="*50)
                print(summary_table.to_string(index=False))
                
                print("\n" + "="*50 + "\nFinal Pop Size Comparison\n" + "="*50)
                print(pop_size_summary_df.to_string(index=False))

                print(f"\nOptimal Uniform Population Size: {optimal_pop_size}")
                print("\n" + "="*50 + "\nPer-Method Optimal Population Sizes\n" + "="*50)
                print(optimal_params_df.to_string(index=False))
        finally:
            sys.stdout = original_stdout

        summary_table.to_csv(csv_summary_phase1_path, index=False, float_format='%.2f')
        optimal_params_df.to_csv(csv_optimal_params_path, index=False, float_format='%.4f')
        pop_size_summary_df.to_csv(csv_comparison_path, index=False, float_format='%.4f')
        
        print("\nAnalysis (best-so-far) completed!")
        print(f"-> Text report: {txt_output_path}")
        print(f"-> Phase 1 Summary (CSV): {csv_summary_phase1_path}")
        print(f"-> Optimal parameters per method (CSV): {csv_optimal_params_path}")
        print(f"-> Pop size comparison table (CSV): {csv_comparison_path}")
        print(f"-> Optimal Uniform Population Size: {optimal_pop_size}")
    else:
        print("Failed to process data or data is empty after processing.")