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
