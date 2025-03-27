import os
import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt

def run_benchmark(width, n, start1,start2, res, v):
    os.chdir("C:\\Users\\vladm\\CLionProjects\\team146\\Implementierung")
    cmd = f"wsl.exe ./prog -d{width[0]},{width[0]} -n{n} -s{start1},{start2} -r{res} -V{v} -B2 2>&1" #Change the number of the -B param to increase the number of images created,
                                                                                                     #the higher the number the more accurate the benchmark
    output = subprocess.run(['cmd', '/c', cmd], capture_output=True, text=True)
    output = output.stdout
    avg=re.findall(r'AVERAGE: (.*) s', output)
    avg=float(avg[0])
    max=re.findall(r'MAX: (.*) s', output)
    max=float(max[0])
    min=re.findall(r'MIN: (.*) s', output)
    min=float(min[0])
    return avg,min,max

def run_sanity_test(width,n,start1,start2,res):
    os.chdir("C:\\Users\\vladm\\CLionProjects\\team146\\Implementierung") #change to your path IF NEEDED
    cmd = f"wsl.exe ./prog -d{width[0]},{width[0]} -n{n} -s{start1},{start2} -r{res} -V0 --test 2>&1"
    output = subprocess.run(['cmd', '/c', cmd], capture_output=True, text=True)
    output = output.stdout
    if output.find("Running tests...\nTEST: Images are equal with epsilon=0.000001\n")==None:
        return 0
    else:
        percent=re.findall(r'TEST: Image difference is (.*)%', output)
        percent=percent[0].split('%')
        percent=float(percent[0])
        print("percent: ", percent)
        return percent

def sanity_test():
    folder_path = "C:\\Users\\vladm\\CLionProjects\\team146\\Implementierung"
    file_path = os.path.join(folder_path, 'sanity_test_data.xlsx')
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'Percentage'])
    width = (800,800)
    n= 50
    start = (-1.77001035,-0.05000005)
    r_list=[4,3.9, 3.1, 2.9 , 2.1 ,1.9 ,1.1 ,0.9, 0.1 , 1e-1 , 0.9e-1,  0.9e-2 , 0.9e-3, 0.9e-4, 0.9e-5 , 0.9e-6]
    for r in r_list:
        percentage = run_sanity_test(width, n, start[0],start[1], r)
        row = {'Breite': width, 'n': n, 's': start, 'r': r, 'Percentage': percentage}
        df = df.append(row, ignore_index=True)
    plt.plot(df['r'], df['Percentage'], 'o-')
    plt.gca().set_xscale('log')
    plt.gcf().set_size_inches(18, 10)
    plt.gca().set_xscale('log')
    plt.gca().invert_xaxis()
    plt.xlabel('Zoom', fontsize=15)
    plt.gca().yaxis.set_label_coords(-0.05, 0.5)
    plt.ylabel('Percentage', fontsize=15)
    plt.title('V0-float vs V0-double', fontsize=20)
    plt.savefig('sanity_test_float_vs_double.png')
    df.to_excel(file_path, index=False)

def run(n):
    if(n=="1"):
        folder_path = "C:\\Users\\vladm\\CLionProjects\\team146\\Implementierung"
        file_path = os.path.join(folder_path, 'benchmark_data.xlsx')
        width_list = [(100,100), (1000, 1000), (3000,3000),(5000,5000), (10000,10000)]
        n_list = [10,50,100]
        start_list = [(-0.3, -0.3)]
        res_list = 4.0
        v_list = [0, 1, 2]
        df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
        for width in width_list:
            for n in n_list:
                row = {'Breite': width[0], 'n': n, 's': start_list[0], 'r': res_list}
                for v in v_list:
                    avg,min,max = run_benchmark(width, n, start_list[0][0],start_list[0][1], res_list, v)
                    row[f'V{v}_AVG'] = avg
                    row[f'V{v}_MIN'] = min
                    row[f'V{v}_MAX'] = max
                df = df.append(row, ignore_index=True)
        start_list = [(-1.75, -0.033)]
        res_list = 0.1
        for width in width_list:
            for n in n_list:
                row = {'Breite': width[0], 'n': n, 's': start_list[0], 'r': res_list}
                for v in v_list:
                    avg, min, max = run_benchmark(width, n, start_list[0][0], start_list[0][1], res_list, v)
                    row[f'V{v}_AVG'] = avg
                    row[f'V{v}_MIN'] = min
                    row[f'V{v}_MAX'] = max
                df = df.append(row, ignore_index=True)
        start_list = [(-1.861, 0.003)]
        res_list = 0.01
        for width in width_list:
            for n in n_list:
                row = {'Breite': width[0], 'n': n, 's': start_list[0], 'r': res_list}
                for v in v_list:
                    avg, min, max = run_benchmark(width, n, start_list[0][0], start_list[0][1], res_list, v)
                    row[f'V{v}_AVG'] = avg
                    row[f'V{v}_MIN'] = min
                    row[f'V{v}_MAX'] = max
                df = df.append(row, ignore_index=True)
        with pd.ExcelWriter(file_path) as writer:
            df.to_excel(writer, index=False, sheet_name='Test1')
    else:
        folder_path = "C:\\Users\\vladm\\CLionProjects\\team146\\Implementierung"
        file_path = os.path.join(folder_path, 'plott.xlsx')
        df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
        width_list = [(500,500), (1000, 1000), (3000, 3000), (5000, 5000),(8000,8000), (10000, 10000)]
        n=40
        start_res=(-0.3, -0.3,4.0)
        v_list = [0, 1, 2]
        for width in width_list:
            row = {'Breite': width[0], 'n': n, 's': f"{start_res[0],start_res[1]}", 'r': start_res[2]}
            for v in v_list:
                avg,min,max = run_benchmark(width, n, start_res[0],start_res[1],start_res[2], v)
                row[f'V{v}_AVG'] = avg
                row[f'V{v}_MIN'] = min
                row[f'V{v}_MAX'] = max
            df = df.append(row, ignore_index=True)
        plt.yscale('log')
        plt.figure(figsize=(15, 8))
        plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
        plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
        plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
        plt.legend()
        plt.title(
            "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
            fontsize=10)
        plt.ylabel('Laufzeit in s')
        plt.savefig('Big ship_dot_line.png')
        df.plot(x='Breite', y=['V0_AVG', 'V1_AVG', 'V2_AVG'], kind='bar', rot=0)
        # set the Title of the plot "Time in seconds"
        plt.title(
            "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
            fontsize=10)
        plt.ylabel('Laufzeit in s')
        plt.savefig('Big ship_bar.png')
        start_res = (-1.861, 0.003, 0.01)
        df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
        for width in width_list:
            row = {'Breite': width[0], 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
            for v in v_list:
                avg, min, max = run_benchmark(width, n, start_res[0], start_res[1], start_res[2], v)
                row[f'V{v}_AVG'] = avg
                row[f'V{v}_MIN'] = min
                row[f'V{v}_MAX'] = max
            df = df.append(row, ignore_index=True)
        plt.yscale('log')
        plt.figure(figsize=(15, 8))
        plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
        plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
        plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
        plt.legend()
        plt.title(
            "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
            fontsize=10)
        plt.ylabel('Laufzeit in s')
        plt.savefig('Medium ship_dot_line.png')
        df.plot(x='Breite', y=['V0_AVG', 'V1_AVG', 'V2_AVG'], kind='bar', rot=0)
        plt.title(
            "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
            fontsize=10)
        plt.ylabel('Laufzeit in s')
        plt.savefig('Medium ship_bar.png')
        plt.show()
        start_res = (-1.75, -0.0330, 0.1)
        df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
        for width in width_list:
            row = {'Breite': width[0], 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
            for v in v_list:
                avg, min, max = run_benchmark(width, n, start_res[0], start_res[1], start_res[2], v)
                row[f'V{v}_AVG'] = avg
                row[f'V{v}_MIN'] = min
                row[f'V{v}_MAX'] = max
            df = df.append(row, ignore_index=True)
        plt.yscale('log')
        plt.figure(figsize=(15, 8))
        plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
        plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
        plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
        plt.legend()
        plt.title(
            "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
            fontsize=10)
        plt.ylabel('Laufzeit in s')
        plt.savefig('Small ship_dot_line.png')
        df.plot(x='Breite', y=['V0_AVG', 'V1_AVG', 'V2_AVG'], kind='bar', rot=0)
        # set the Title of the plot "Time in seconds"
        plt.title(
            "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
            fontsize=10)
        plt.ylabel('Laufzeit in s')
        plt.savefig('Small ship_bar.png')


def plot_step100():
    width=200
    n=40
    start_res=(-0.3, -0.3,4.0)
    v_list=[0,1,2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width<=2000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l=(width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width+=100
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Big100.png')
    width = 200
    start_res = (-1.861, 0.003, 0.01)
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width <= 2000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width += 100
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Medium100.png')
    width = 200
    start_res = (-1.75, -0.0330,0.1)
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width <= 2000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width += 100
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Small100.png')


def plot_step50():
    width = 200
    n = 40
    start_res = (-0.3, -0.3, 4.0)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width <= 2000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width += 50
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    df.plot(x='Breite', y=['V0_AVG', 'V1_AVG', 'V2_AVG'], kind='line', rot=0)
    # set the Title of the plot "Time in seconds"
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Big50.png')
    width = 200
    start_res = (-1.861, 0.003, 0.01)

    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width <= 2000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width += 50
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Medium50.png')
    width = 200
    n = 40
    start_res = (-1.75, -0.0330, 0.1)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width <= 2000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width += 50
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Small50.png')

def plot_step25():
    width = 600
    n = 40
    start_res = (-0.3, -0.3, 4.0)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width <= 1000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width += 25
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    df.plot(x='Breite', y=['V0_AVG', 'V1_AVG', 'V2_AVG'], kind='line', rot=0)
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Big25.png')
    width = 600
    n = 40
    start_res = (-1.861, 0.003, 0.01)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width <= 1000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width += 25
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Medium25.png')
    width = 600
    n = 40
    start_res = (-1.75, -0.0330, 0.1)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while width <= 1000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        width += 25
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['Breite'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['Breite'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['Breite'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Iterationen: " + f"{n}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressive_Small25.png')

def progressive_plot_n():
    width = 800
    n = 1
    start_res = (-0.3, -0.3, 4.0)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while n <= 5000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        n += 100
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['n'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['n'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['n'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Breite: " + f"{width}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressiveBig_n.png')
    width = 800
    n = 1
    start_res = (-1.861, 0.003, 0.01)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while n <= 5000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        n += 100
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['n'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['n'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['n'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Breite: " + f"{width}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressiveMedium_n.png')
    width = 800
    n = 1
    start_res = (-1.75, -0.0330, 0.1)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while n <= 5000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        n += 100
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['n'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['n'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['n'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Breite: " + f"{width}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.savefig('progressiveSmall_n.png')

def test_at_0_0():
    width = 800
    n = 1
    start_res = (0,0,0.1)
    v_list = [0, 1, 2]
    df = pd.DataFrame(columns=['Breite', 'n', 's', 'r', 'V0_AVG','V0_MIN','V0_MAX', 'V1_AVG','V1_MIN','V1_MAX', 'V2_AVG','V2_MIN','V2_MAX',])
    while n <= 5000:
        row = {'Breite': width, 'n': n, 's': f"{start_res[0], start_res[1]}", 'r': start_res[2]}
        for v in v_list:
            width_l = (width, width)
            avg, min, max = run_benchmark(width_l, n, start_res[0], start_res[1], start_res[2], v)
            row[f'V{v}_AVG'] = avg
            row[f'V{v}_MIN'] = min
            row[f'V{v}_MAX'] = max
        df = df.append(row, ignore_index=True)
        n += 100
    plt.yscale('log')
    plt.figure(figsize=(15, 8))
    plt.plot(df['n'], df['V0_AVG'], 'o-', markersize=10, label='V0_AVG')
    plt.plot(df['n'], df['V1_AVG'], 'o-', markersize=10, label='V1_AVG')
    plt.plot(df['n'], df['V2_AVG'], 'o-', markersize=10, label='V2_AVG')
    plt.legend()
    plt.title(
        "Breite: " + f"{width}" + " Start: " + f"{start_res[0], start_res[1]}" + " Resolution: " + f"{start_res[2]}",
        fontsize=10)
    plt.ylabel('Laufzeit in s')
    plt.xlabel('Iterationen n')
    plt.savefig('progressive_n.png')




if __name__ == "__main__":
    n = []
    while "Q" not in n:
        a = input("Benchmark - 1\nPlot - 2\nprogressive Plot 100 - 3\nprogressive Plot 50 - 4\nprogressive Plot 25 steps - 5\nprogressive Plot 100-n - 6\nfull black picture with progressive n iterations - 7\nsanity check - 8\n")
        if "Q" in a:
            n = a.split()
            if "Q" in n:
                n.remove("Q")
            break
    n = list(dict.fromkeys(n)) #remove duplicates
    for n in n:
        if n == "1":
            run("1")
        elif n == "2":
            run("2")
        elif n == "3":
            plot_step100()
        elif n == "4":
            plot_step50()
        elif n == "5":
            plot_step25()
        elif n == "6":
            progressive_plot_n()
        elif n == "7":
            test_at_0_0()
        elif n == "8":
            sanity_test()
