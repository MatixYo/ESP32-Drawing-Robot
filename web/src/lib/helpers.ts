import { Config } from '../types/config';
import { Position } from '../types/position';

export function sleep(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}

export function nmap(
  x: number,
  inMin: number,
  inMax: number,
  outMin: number,
  outMax: number,
): number {
  return ((x - inMin) * (outMax - outMin)) / (inMax - inMin) + outMin;
}

export function normalizePosition(
  e: React.PointerEvent<HTMLDivElement>,
  config: Config,
): Position {
  const rect = e.currentTarget.getBoundingClientRect();
  const x = nmap(
    e.clientX - rect.left,
    0,
    rect.width,
    config.minX,
    config.maxX,
  );
  const y = nmap(
    e.clientY - rect.top,
    0,
    rect.height,
    config.minY,
    config.maxY,
  );
  return { x, y };
}

export function downloadFile(
  filename: string,
  data: string,
  type = 'text/plain',
) {
  const a = document.createElement('a');
  const file = new Blob([data], { type });
  a.href = URL.createObjectURL(file);
  a.download = filename;
  a.click();
  URL.revokeObjectURL(a.href);
  a.remove();
}

export function readFile(accept: string[]): Promise<string> {
  return new Promise(resolve => {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = accept.map(a => `.${a}`).join(',');
    input.onchange = () => {
      const file = input.files?.[0];
      if (!file) return resolve('');
      const reader = new FileReader();
      reader.onload = () => {
        resolve(reader.result as string);
      };
      reader.readAsText(file);
    };
    input.click();
  });
}
