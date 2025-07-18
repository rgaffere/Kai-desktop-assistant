import { appWindow } from "@tauri-apps/api/window";
import { LogicalSize, PhysicalPosition } from "@tauri-apps/api/dpi";
import { currentMonitor } from "@tauri-apps/api/window";

const overlayEl = document.getElementById("overlay");

async function fetchOverlayText() {
  try {
    const res = await fetch("overlay.json?_=" + Date.now());
    const data = await res.json();
    overlayEl.innerText = data.text || "(no text)";
    waitAndResize();
  } catch (e) {
    overlayEl.innerText = "No data yet.";
  }
}

function waitAndResize() {
  requestAnimationFrame(() => {
    requestAnimationFrame(() => autoResizeAndMove());
  });
}

async function autoResizeAndMove() {
  const rect = overlayEl.getBoundingClientRect();
  const scale = window.devicePixelRatio || 1;
  const width = Math.ceil(rect.width * scale);
  const height = Math.ceil(rect.height * scale);

  await appWindow.setSize(new LogicalSize(width, height));

  const monitor = await currentMonitor();
  if (monitor) {
    const screenWidth = monitor.size.width;
    const x = screenWidth - width - 20;
    const y = 20;
    await appWindow.setPosition(new PhysicalPosition(x, y));
  }
}

fetchOverlayText();
setInterval(fetchOverlayText, 1000);
