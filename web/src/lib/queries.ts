import { Config } from '../types/config';
import { Position } from '../types/position';
import { Status } from '../types/status';
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

export function print(g: string[]) {
  return post('/print', { gcode: g.join('\n') });
}

export function home() {
  return sendGCode('G28');
}

// Queries
export function getStatus(): Promise<Status> {
  return get<Status>('/status');
}

export function getConfig(): Promise<Config> {
  return get<Config>('/config');
}
