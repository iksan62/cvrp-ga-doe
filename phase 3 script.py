import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import matplotlib as mpl
from scipy.stats import wilcoxon

mpl.rcParams['font.family'] = 'Times New Roman'
mpl.rcParams['font.size'] = 10
mpl.rcParams['axes.linewidth'] = 0.75

def run_phase3_analysis(input_path, output_dir):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir, exist_ok=True)

    bks_data = {
        'X-n106-k14': 26362, 'X-n204-k19': 19565, 'X-n308-k13': 25859,
        'X-n376-k94': 147713, 'X-n502-k39': 69226, 'X-n655-k131': 106780,
        'X-n801-k40': 73305, 'X-n876-k59': 99299, 'X-n957-k87': 85465,
        'X-n1001-k43': 72355
    }

    try:
        df = pd.read_csv(input_path, sep=',')
    except:
        df = pd.read_csv(input_path, sep=r'\s+', engine='python')

    df.columns = df.columns.str.strip()

    meta_cols = ['Benchmark', 'type_konfig_parameter', 'Run']
    method_cols = [c for c in df.columns if c not in meta_cols]

    df_long = df.melt(id_vars=meta_cols, value_vars=method_cols, var_name='Method', value_name='Fitness')
    df_long['BKS'] = df_long['Benchmark'].map(bks_data)
    df_long.dropna(subset=['BKS'], inplace=True)
    df_long['RPD'] = ((df_long['Fitness'] - df_long['BKS']) / df_long['BKS']) * 100

    table_summary = df_long.groupby(['Method', 'type_konfig_parameter'])['RPD'].mean().unstack()

    col_mapping = {
        'untuned': 'Untuned_RPD',
        'tuned type 2': 'Tuned_Fair_RPD',
        'tuned type 1': 'Tuned_Best_RPD'
    }
    
    existing_cols = [c for c in col_mapping.keys() if c in table_summary.columns]
    table_summary = table_summary[existing_cols].rename(columns=col_mapping)

    if 'Untuned_RPD' in table_summary.columns and 'Tuned_Fair_RPD' in table_summary.columns:
        table_summary['Efficiency_Gain_Percent'] = ((table_summary['Untuned_RPD'] - table_summary['Tuned_Fair_RPD']) / table_summary['Untuned_RPD']) * 100

    methods = table_summary.index.tolist()
    p_values = {}
    
    for method in methods:
        data_untuned = df_long[(df_long['Method'] == method) & (df_long['type_konfig_parameter'] == 'untuned')]['RPD'].reset_index(drop=True)
        data_tuned_fair = df_long[(df_long['Method'] == method) & (df_long['type_konfig_parameter'] == 'tuned type 2')]['RPD'].reset_index(drop=True)
        
        if len(data_untuned) > 0 and len(data_tuned_fair) > 0:
            min_len = min(len(data_untuned), len(data_tuned_fair))
            try:
                stat, p_val = wilcoxon(data_untuned[:min_len], data_tuned_fair[:min_len], alternative='greater')
                p_values[method] = p_val
            except ValueError:
                p_values[method] = np.nan
        else:
            p_values[method] = np.nan

    table_summary['Wilcoxon_p_value'] = table_summary.index.map(p_values)
    
    if 'Tuned_Best_RPD' in table_summary.columns:
        table_summary = table_summary.sort_values('Tuned_Best_RPD', ascending=True)

    output_csv_path = os.path.join(output_dir, 'Table8_Efficiency_Analysis.csv')
    table_summary.to_csv(output_csv_path, float_format='%.6f')
    
    with open(os.path.join(output_dir, 'phase3_report.txt'), 'w', encoding='utf-8') as f:
        f.write("PHASE 3 ANALYSIS REPORT\n")
        f.write("=======================\n\n")
        f.write(table_summary.to_string())
        f.write("\n\nAnalysis completed successfully.")

    print(f"Data processing completed. Results saved to {output_dir}")

    plt.figure(figsize=(11, 7))
    plot_cols = [c for c in table_summary.columns if 'RPD' in c]
    df_plot = table_summary[plot_cols].reset_index()

    df_plot_melted = df_plot.melt(id_vars='Method', value_vars=plot_cols, var_name='Configuration', value_name='Mean_RPD')

    label_map = {
        'Untuned_RPD': 'Untuned (Pop=20, Gen=1000)',
        'Tuned_Fair_RPD': 'Tuned Fair (Pop=300, Gen=67)',
        'Tuned_Best_RPD': 'Tuned Best (Pop=300, Gen=500)'
    }
    df_plot_melted['Configuration_Label'] = df_plot_melted['Configuration'].map(label_map)

    palette_colors = {
        label_map.get('Untuned_RPD'): '#e74c3c',
        label_map.get('Tuned_Fair_RPD'): '#f1c40f',
        label_map.get('Tuned_Best_RPD'): '#27ae60'
    }

    ax = sns.barplot(
        data=df_plot_melted,
        x='Method',
        y='Mean_RPD',
        hue='Configuration_Label',
        palette=palette_colors,
        edgecolor='black',
        linewidth=0.5
    )

    plt.title('Impact of Parameter Tuning: Efficiency vs. Effectiveness Analysis', fontsize=12, fontweight='bold')
    plt.ylabel('Average Relative Percentage Deviation (RPD %)', fontsize=10)
    plt.xlabel('Initialization Method', fontsize=10)
    plt.legend(title='Parameter Configuration')
    plt.grid(axis='y', linestyle='--', alpha=0.3)

    for container in ax.containers:
        ax.bar_label(container, fmt='%.1f', padding=3, fontsize=8)

    plt.tight_layout()
    
    output_svg_path = os.path.join(output_dir, 'Figure3_Comparative_Analysis.svg')
    output_png_path = os.path.join(output_dir, 'Figure3_Comparative_Analysis.png')
    plt.savefig(output_svg_path, dpi=300)
    plt.savefig(output_png_path, dpi=300)
    
    print(f"Figures saved to {output_dir}")
    plt.show()

if __name__ == "__main__":
    input_data_path = os.path.join('D:\\', 'dat3-doe.csv')
    output_directory = os.path.join('D:\\', 'phase_3')
    run_phase3_analysis(input_data_path, output_directory)