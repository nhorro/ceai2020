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
        [FN,TN]
    ])
    seaborn.set(color_codes=True)
    plt.figure(1, figsize=(9, 6)) 
    plt.title(title) 

    seaborn.set(font_scale=1.4)
    ax = seaborn.heatmap(data, annot=True, cmap="YlGnBu", cbar_kws={'label': 'Scale'}, fmt='g')
    ax.set_yticklabels(["True","False"])
    ax.set_xticklabels(["True","False"])

    ax.set(ylabel="Predicted Label", xlabel="Actual Label")
    return TP,TN,FP,FN
