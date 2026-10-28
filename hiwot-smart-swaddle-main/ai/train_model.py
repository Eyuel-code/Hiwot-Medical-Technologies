"""Baseline apnea model for Hiwot (work in progress, ~45% per AI progress report, Aug 29 2026).

Loads a PhysioNet PICSDB respiration record, slices it into 150-sample windows and
builds a small 1D-CNN. NOTE: labels are placeholders (all zeros = normal), so this
script does not yet train a useful model. Real labels + the training loop are the
next milestone.
"""
import os
import wfdb
import numpy as np
from tensorflow.keras import layers, models

DATA_DIR = "./preterm-infant-cardio-respiratory-signals-database-1.0.0"
record = wfdb.rdrecord(os.path.join(DATA_DIR, "infant1_resp"))
respiration_signal = record.p_signal
print("Successfully loaded signal shape:", respiration_signal.shape)


def create_windows(signal_data, window_size=150):
    windows = []
    for i in range(0, len(signal_data) - window_size, window_size):
        windows.append(signal_data[i:i + window_size])
    return np.array(windows)


X_train = create_windows(respiration_signal)
y_train = np.zeros(len(X_train))  # placeholder labels (0 = normal)

model = models.Sequential([
    layers.Input(shape=(150, X_train.shape[2] if len(X_train.shape) > 2 else 1)),
    layers.Conv1D(16, kernel_size=3, activation="relu"),
    layers.MaxPooling1D(pool_size=2),
    layers.Conv1D(32, kernel_size=3, activation="relu"),
    layers.GlobalAveragePooling1D(),
    layers.Dense(16, activation="relu"),
    layers.Dense(1, activation="sigmoid"),  # anomaly probability 0.0 .. 1.0
])
model.compile(optimizer="adam", loss="binary_crossentropy", metrics=["accuracy"])
print("Model architecture compiled successfully and ready for training execution!")
