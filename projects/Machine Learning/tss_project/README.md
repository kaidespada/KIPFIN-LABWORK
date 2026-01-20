tts_project/
│
├── data/
│   ├── raw/                    # Сырые аудио и тексты
│   │   ├── audio/
│   │   │   ├── sample_001.wav
│   │   │   ├── sample_002.wav
│   │   │   └── ...
│   │   └── transcripts.txt     # Текстовые транскрипции
│   │
│   ├── processed/              # Обработанные данные
│   │   ├── mel_spectrograms/
│   │   └── text_sequences/
│   │
│   └── splits/                 # Разделение на train/val/test
│       ├── train.txt
│       ├── val.txt
│       └── test.txt
│
├── models/
│   ├── __init__.py
│   ├── encoder.py              # Текстовый энкодер
│   ├── decoder.py              # Аудио декодер
│   ├── tts_model.py            # Основная модель
│   └── vocoder.py              # Vocoder для генерации аудио
│
├── utils/
│   ├── __init__.py
│   ├── audio_processing.py     # Работа с аудио (mel-спектрограммы)
│   ├── text_processing.py      # Работа с текстом (токенизация)
│   └── metrics.py              # Метрики качества
│
├── dataset/
│   ├── __init__.py
│   └── tts_dataset.py          # PyTorch Dataset класс
│
├── training/
│   ├── __init__.py
│   ├── trainer.py              # Основной класс для обучения
│   └── config.py               # Конфигурация обучения
│
├── inference/
│   ├── __init__.py
│   └── synthesizer.py          # Синтез речи из текста
│
├── checkpoints/                # Сохраненные веса модели
│   ├── model_epoch_10.pth
│   └── best_model.pth
│
├── outputs/                    # Сгенерированные аудио
│   └── generated_audio/
│
├── notebooks/                  # Jupyter ноутбуки для экспериментов
│   ├── data_exploration.ipynb
│   └── model_testing.ipynb
│
├── train.py                    # Скрипт для запуска обучения
├── inference.py                # Скрипт для генерации речи
├── preprocess.py               # Предобработка данных
├── requirements.txt            # Зависимости проекта
├── config.yaml                 # Общая конфигурация
└── README.md                   # Документация проекта