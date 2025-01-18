import { Config } from '../types/config';
import { Position } from '../types/position';
import { post, get } from './http-client';

function sendGCode(g: string) {
  return post('/gcode', { gcode: g });
}

// Actions
export function move(pos: Position) {
  return sendGCode(`G0 X${pos.x} Y${pos.y}`);
}

export function raiseTool(raise = true) {
  return sendGCode(raise ? 'M5' : 'M3');
}

export function assembly() {
  return post('/assembly');
}

export function restart() {
  return post('/restart');
}

export function print(gcode: string[]) {
  return post('/print', { gcode: [...gcode, 'G28'].join('\n') });
}

export function home() {
  return sendGCode('G28');
}

// Queries
export function getState() {
  return get('/state');
}

export function getConfig(): Promise<Config> {
  return Promise.resolve({
    minX: -50,
    minY: 25,
    maxX: 50,
    maxY: 125,
    homeX: 0,
    homeY: 25,
  });
  return get<Config>('/config');
}
