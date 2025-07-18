# Kai – Desktop AI Assistant (Prototype)

**Kai** is a real-time desktop assistant that captures screen content and audio, interprets the user’s environment, and responds through an intelligent overlay and voice. It was built as a personal project to explore a multimodal J.A.R.V.I.S.-like assistant.

Although functional, **Kai is now deprecated** as development has shifted to a more advanced successor system.

---

## 🧠 Features

- 🎤 **Voice & Screen Context**: Captures screen via macOS CoreGraphics and voice (placeholder) to form a prompt.
- 🤖 **Intent Classification**: Lightweight Python classifier decides if GPT should be called or action taken locally.
- 💬 **GPT-4o Integration**: Streamed responses using OpenAI’s GPT-4o with Node.js and Axios.
- 🗣️ **Text-to-Speech (TTS)**: Real-time ElevenLabs voice playback with stream-based synthesis.
- 💻 **Overlay UI**: JSON-driven response display rendered by a Tauri-based frontend.
  
---

## ⚙️ Requirements

- macOS (for screen capture APIs)
- C++17 compiler (e.g. clang)
- Node.js ≥ 18
- Python ≥ 3.8
- Dependencies:
  - [Tesseract OCR](https://github.com/tesseract-ocr/tesseract)
  - [ElevenLabs API key](https://www.elevenlabs.io/)
  - [OpenAI API key](https://platform.openai.com/)
  - Tauri frontend running in `kai-overlay`

---

## ✍️ Author

Created by **Ryan G** as a personal project and experiment in real-time AI interface design.
