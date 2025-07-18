const axios = require('axios');
const fs = require('fs');
const player = require('play-sound')();
require('dotenv').config({path: "../.env"});

const elevenKey = process.env.ELEVENLABS_API_KEY;
const voiceId = process.env.VOICE_ID;
const TMP_AUDIO = "/tmp/kai-speak.mp3";

const text = process.argv.slice(2).join(" ");

if (!text || !elevenKey) {
  console.error("[Kai] Missing text or ELEVENLABS_API_KEY");
  process.exit(1);
}

axios({
  method: "POST",
  url: `https://api.elevenlabs.io/v1/text-to-speech/${voiceId}/stream`,
  headers: {
    "xi-api-key": elevenKey,
    "Content-Type": "application/json"
  },
  data: {
    text: text,
    model_id: "eleven_multilingual_v2",
    voice_settings: {
      stability: 0.3,
      similarity_boost: 0.8
    }
  },
  responseType: "stream"
})
  .then(response => {
    const writer = fs.createWriteStream(TMP_AUDIO);
    response.data.pipe(writer);
    writer.on("finish", () => {
      player.play(TMP_AUDIO, err => {
        if (err) console.error("[Kai] Playback error:", err);
      });
    });
  })
  .catch(err => {
    console.error("[Kai] ElevenLabs streaming error:", err.message);
  });
