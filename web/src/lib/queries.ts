import { Config } from '../types/config';
import { Position } from '../types/position';
import { Status } from '../types/status';
import { moveGCode } from './gcode';
import { post, get } from './http-client';

function sendGCode(g: string | string[]) {
  const gcode = Array.isArray(g) ? g.join('\n') : g;
  return post('/gcode', { gcode });
}

// Actions
export function move(pos: Position) {
  return sendGCode(moveGCode(pos));
}

export function raiseTool(raise = true) {
  return sendGCode(raise ? 'M5' : 'M3');
}

export function home() {
  return sendGCode('G28');
}

export function restart() {
  return sendGCode('M999');
}

export function assembly() {
  return post('/assembly');
}

export function print(g: string[]) {
  return sendGCode([...g, 'G28']);
}

// Queries
export function getStatus(): Promise<Status> {
  return get<Status>('/status');
}

export function getConfig(): Promise<Config> {
  return get<Config>('/config');
}
