require('dotenv').config({ path: "../.env" });
const axios = require('axios');
const fs = require('fs');
const path = require('path');

const apiKey = process.env.OPENAI_API_KEY;
const model = process.env.OPENAI_MODEL || "gpt-4o";
const input = process.argv.slice(2).join(" ");

if (!input || !apiKey) {
  process.exit(1);
}

const payload = {
  model: model,
  messages: [
    {
      role: "system",
      content: "You are Kai, a real-time desktop assistant who interprets the user’s environment like JARVIS. You analyze context from screen text, browser tabs, and voice commands. Identify what the user is trying to do or say and respond only when it’s truly helpful. If input is random or low-value (like a search bar with 'repeat after me'), ignore or respond minimally. Be concise, sharp, and slightly witty if appropriate."
    },
    {
      role: "user",
      content: input
    }
  ],
  temperature: 0.7,
  stream: true,
  max_tokens: 60
};

axios({
  method: "post",
  url: "https://api.openai.com/v1/chat/completions",
  headers: {
    Authorization: `Bearer ${apiKey}`,
    "Content-Type": "application/json"
  },
  data: payload,
  responseType: "stream"
})
  .then(response => {
    let fullText = "";

    response.data.on("data", chunk => {
      const lines = chunk.toString().split('\n').filter(line => line.trim().startsWith('data:'));
      for (const line of lines) {
        const message = line.replace(/^data:\s*/, '');
        if (message === "[DONE]") return;

        try {
          const parsed = JSON.parse(message);
          const delta = parsed.choices?.[0]?.delta?.content;
          if (delta) fullText += delta;
        } catch {}
      }
    });

    response.data.on("end", () => {
      const overlayPath = path.resolve(__dirname, "../../kai-overlay/src/overlay.json");
      fs.writeFileSync(overlayPath, JSON.stringify({ text: fullText }, null, 2));
    });
  })
  .catch(() => {
    // Fail silently
  });
