import { Config } from '../types/config';
import { Position } from '../types/position';
import { Status } from '../types/status';
import { moveGCode } from './gcode';
import { post, get } from './http-client';
import { isToolLowered } from './gcode';

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
  return post('/restart');
}

export function assembly() {
  return post('/assembly');
}

export function print(g: string[]) {
  const gcode = [...g];
  isToolLowered(gcode) && gcode.push('M5');
  return sendGCode([...gcode, 'G28']);
}

// Queries
export function getStatus(): Promise<Status> {
  return get<Status>('/status');
}

export function getConfig(): Promise<Config> {
  return get<Config>('/config');
}
