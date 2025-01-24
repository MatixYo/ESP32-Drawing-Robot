import { useReducer } from 'react';

type GCodeState = string[];

type GCodeAction =
  | { type: 'SET_GCODE'; payload: string[] }
  | { type: 'ADD_GCODE_LINE'; payload: string | string[] }
  | { type: 'CLEAR_LINE' }
  | { type: 'CLEAR_GCODE' };

function gcodeReducer(state: GCodeState, action: GCodeAction): GCodeState {
  switch (action.type) {
    case 'SET_GCODE':
      return action.payload;
    case 'ADD_GCODE_LINE': {
      const lines = Array.isArray(action.payload)
        ? action.payload
        : [action.payload];
      return [...state, ...lines];
    }
    case 'CLEAR_LINE':
      return state.slice(0, -1);
    case 'CLEAR_GCODE':
      return [];
    default:
      return state;
  }
}

export function useGCode() {
  const [gcode, dispatch] = useReducer(gcodeReducer, []);

  const setGCode = (gcode: string[]) =>
    dispatch({ type: 'SET_GCODE', payload: gcode });

  const addLine = (line: string | string[]) =>
    dispatch({ type: 'ADD_GCODE_LINE', payload: line });

  const clearLine = () => dispatch({ type: 'CLEAR_LINE' });

  const clearAll = () => dispatch({ type: 'CLEAR_GCODE' });

  return {
    gcode,
    setGCode,
    addLine,
    clearLine,
    clearAll,
  };
}
