import random
import subprocess
import concurrent.futures
import os

def run_instance(params):
    """
    執行 ./main 並傳入參數。
    :param params: 包含參數的字串列表，例如 ["--seed", "123", "--popSize", "300", ...]
    :return: 回傳執行結果的標準輸出與標準錯誤訊息。
    """
    command = ["./main"] + params
    result = subprocess.run(command, capture_output=True, text=True)
    return result.stdout, result.stderr

parameter_sets = []

def add_params(k, m_range):
    """依據給定的 k 值、m 的範圍和 bound，產生參數集合並加入列表"""
    for m in m_range:
        parameter_sets.append([
            "--k", str(k),
            "--m", str(m),
        ])

def main():
    for run in range(50):
        for fun in range(1, 31):
            for m in range(10, 60, 10):
                temp = []
                temp.append("--funNum")
                temp.append(str(fun))
                temp.append("--m")
                temp.append(str(m))
                parameter_sets.append(temp)

    # 設定 ThreadPoolExecutor 的 max_workers 為當前 CPU 的核心數
    max_workers = os.cpu_count()
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = {executor.submit(run_instance, params): params for params in parameter_sets}
        for future in concurrent.futures.as_completed(futures):
            params = futures[future]
            try:
                stdout, stderr = future.result()
                print("參數組合:", params)
                print("輸出:\n", stdout)
                if stderr:
                    print("錯誤訊息:\n", stderr)
            except Exception as e:
                print("參數組合", params, "執行時發生例外:", e)

if __name__ == "__main__":
    main()
 