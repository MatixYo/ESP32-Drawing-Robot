import { Config } from '../types/config';
import { Position } from '../types/position';

export function sleep(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}

export function f(value: number): string {
  return value.toFixed(1);
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
