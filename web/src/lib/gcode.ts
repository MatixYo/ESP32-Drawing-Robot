import { Position } from '../types/position';

export function isToolLowered(gcode: string[]): boolean {
  const line = [...gcode].reverse().find(c => ['M3', 'M5'].includes(c));
  return line === 'M3';
}

interface GCodeLine {
  cmd: string;
  params: Record<string, number>;
}

export function parseGCodeLine(line: string): GCodeLine {
  const [cmd, ...rest] = line.split(' ');
  const params = rest.reduce<Record<string, number>>((acc, param) => {
    acc[param.slice(0, 1)] = parseFloat(param.slice(1));
    return acc;
  }, {});

  return { cmd, params };
}

export function f(value: number): string {
  return value.toFixed(1);
}

export function moveGCode(pos: Position): string {
  return `G1 X${f(pos.x)} Y${f(pos.y)}`;
}
