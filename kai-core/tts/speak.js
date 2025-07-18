// kai-core/tts/speak.js

const axios = require('axios');
const fs = require('fs');
const path = require('path');
const player = require('play-sound')();

const API_KEY = process.env.ELEVENLABS_API_KEY; // keep your key in env
const VOICE_ID = 'EXAVITQu4vr4xnSDxMaL'; // default voice

const text = process.argv.slice(2).join(" ");

async function speak(text) {
  try {
    const response = await axios({
      method: 'POST',
      url: `https://api.elevenlabs.io/v1/text-to-speech/${VOICE_ID}`,
      headers: {
        'xi-api-key': API_KEY,
        'Content-Type': 'application/json'
      },
      data: {
        text: text,
        model_id: "eleven_monolingual_v1",
        voice_settings: {
          stability: 0.5,
          similarity_boost: 0.75
        }
      },
      responseType: 'stream'
    });

    const audioPath = path.resolve(__dirname, 'kai-response.mp3');
    const writer = fs.createWriteStream(audioPath);

    response.data.pipe(writer);
    writer.on('finish', () => {
      player.play(audioPath, function (err) {
        if (err) console.error("[Kai] Audio playback error:", err);
      });
    });
  } catch (err) {
    console.error("[Kai] ElevenLabs TTS failed:", err.message);
  }
}

speak(text);
