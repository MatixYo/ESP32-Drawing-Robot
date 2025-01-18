export function isToolLowered(gcode: string[]): boolean {
  const line = [...gcode].reverse().find(c => ['M3', 'M5'].includes(c));
  return line === 'M3';
}
