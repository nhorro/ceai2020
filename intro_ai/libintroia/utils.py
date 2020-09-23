import numpy as np
import matplotlib.pyplot as plt
import seaborn

def plot_confusion_matrix(predicted,actual,title="Confusion Matrix"):
    TP = np.count_nonzero( (predicted == actual) & (predicted==True))
    TN = np.count_nonzero( (predicted == actual) & (predicted==False))
    FP = np.count_nonzero( (predicted != actual) & (predicted==True))
    FN = np.count_nonzero( (predicted != actual) & (predicted==False))
    data = np.array([
        [TP,FP],
        [TN,FN]
    ])
    seaborn.set(color_codes=True)
    plt.figure(1, figsize=(9, 6)) 
    plt.title(title) 
    labels=["True","False","True","False"]
    seaborn.set(font_scale=1.4)
    ax = seaborn.heatmap(data, annot=True, cmap="YlGnBu", cbar_kws={'label': 'Scale'}, fmt='g')
    ax.set_xticklabels(labels)
    ax.set_yticklabels(labels) 
    ax.set(ylabel="True Label", xlabel="Predicted Label")
