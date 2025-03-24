import subprocess
import re
import random
from itertools import permutations
import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm
from scipy.stats import chi2  # 用於計算卡方分布的 p-value

# -------------------------
# 測試 shuffle 次數
test_count = 1000000

# 建立初始命令字串：先建立一個 queue (new, it 1, ..., it 4)
cmd_input = "new\nit 1\nit 2\nit 3\nit 4\n"
# 加入多次 shuffle 命令
for i in tqdm(range(test_count)):
    cmd_input += "shuffle\n"
cmd_input += "free\nquit\n"

# -------------------------
# 呼叫外部程式 qtest 並取得輸出結果
command = './qtest -v 3'
clist = command.split()
completedProcess = subprocess.run(clist, capture_output=True, text=True, input=cmd_input)
output_str = completedProcess.stdout

# -------------------------
# 從輸出結果中擷取 shuffle 結果字串
startIdx = output_str.find("l = [1 2 3 4]")
endIdx = output_str.find("l = NULL")
# 從 "l = [1 2 3 4]" 後開始，剝離前面的部分
result_str = output_str[startIdx + 14 : endIdx]

# 使用正則表達式擷取所有排列結果 (例如：'1 2 3 4')
regex = re.compile(r'\d \d \d \d')
result_matches = regex.findall(result_str)

# -------------------------
# 將結果字串轉換成列表格式，每筆結果為 list，例如 ['1','2','3','4']
observed_results = [match.split() for match in result_matches]

# -------------------------
# 建立所有可能的排列 (共 24 種排列)
def all_permutations(nums):
    return list(permutations(nums, len(nums)))

base_array = ['1', '2', '3', '4']
all_perm = all_permutations(base_array)

# 初始化 counterSet 字典，key 為排列字串，value 為出現次數
counterSet = {}
for perm in all_perm:
    key = ''.join(perm)
    counterSet[key] = 0

# 計算每一種 shuffle 結果的數量
for obs in observed_results:
    key = ''.join(obs)
    if key in counterSet:
        counterSet[key] += 1
    else:
        # 理論上不會發生，但若有不符格式，可印出警告
        print("Warning: unexpected permutation", key)

# -------------------------
# 卡方檢定計算
total_permutations = len(all_perm)  # 應為 24
expectation = test_count / total_permutations  # 每個排列理論期望次數
chiSquareSum = sum(((count - expectation) ** 2) / expectation for count in counterSet.values())
dof = total_permutations - 1  # 自由度，24 - 1 = 23
p_value = chi2.sf(chiSquareSum, dof)  # sf = 1 - cdf

print("Expectation (each): ", expectation)
print("Observation: ", counterSet)
print("Chi-square sum: ", chiSquareSum)
print("Degrees of freedom: ", dof)
print("p-value: ", p_value)

# -------------------------
# 產生圖表
# 1. 條形圖：每個排列出現次數
labels = list(counterSet.keys())
counts = list(counterSet.values())
x = np.arange(len(labels))

plt.figure(figsize=(12, 6))
plt.bar(x, counts, color='skyblue', edgecolor='black')
plt.axhline(expectation, color='red', linestyle='--', label=f'Expectation = {expectation:.0f}')
plt.xticks(x, labels)
plt.xlabel('Permutation')
plt.ylabel('Counts')
plt.title('Shuffle Result Distribution')
plt.legend()
plt.tight_layout()
plt.savefig('Shuffle_Result_Distribution.png')  # 儲存條形圖
plt.close()

# 2. 卡方分布圖：顯示卡方統計量與 p-value 區間
x_vals = np.linspace(0, chi2.ppf(0.995, dof), 500)
pdf_vals = chi2.pdf(x_vals, dof)
plt.figure(figsize=(8, 5))
plt.plot(x_vals, pdf_vals, 'b-', lw=2, label='Chi-square PDF')
plt.fill_between(x_vals, pdf_vals, where=(x_vals>=chiSquareSum), color='red', alpha=0.5,
                 label=f'p-value = {p_value:.3f}')
plt.axvline(chiSquareSum, color='red', linestyle='--', lw=2, label=f'Chi-square stat = {chiSquareSum:.2f}')
plt.xlabel('Chi-square Statistic')
plt.ylabel('Probability Density')
plt.title('Chi-square Distribution (df={})'.format(dof))
plt.legend()
plt.tight_layout()
plt.savefig('Chi-square_Distribution.png')  # 儲存卡方分布圖
plt.close()
