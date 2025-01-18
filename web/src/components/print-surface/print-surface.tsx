import { useState } from 'react';
import { f, nmap, normalizePosition } from '../../lib/helpers';
import { Config } from '../../types/config';
import { Position } from '../../types/position';
import { Canvas } from './canvas';
import s from './print-surface.module.css';
import { Switch } from '../button/switch';
import { isToolLowered } from '../../lib/gcode';

interface PrintSurfaceProps {
  config: Config;
  toolPosition: Position;
  setToolPosition: (position: Position) => void;
  gcode: string[];
  addGCode: (line: string) => void;
}

type Mode = 'move' | 'line' | 'circle';

export function PrintSurface({
  config,
  toolPosition,
  setToolPosition,
  gcode,
  addGCode,
}: PrintSurfaceProps) {
  const width = config.maxX - config.minX;
  const height = config.maxY - config.minY;

  const [mode, setMode] = useState<Mode>('move');

  const handleModeChange = (mode: Mode) => {
    if (isToolLowered(gcode)) {
      addGCode('M5');
    }
    setMode(mode);
  };

  const handleClick = (e: React.PointerEvent<HTMLDivElement>) => {
    const positon = normalizePosition(e, config);

    if (mode === 'move') {
      setToolPosition(positon);
    }
    if (mode === 'line') {
      if (!isToolLowered(gcode)) {
        addGCode('M3');
      }
      addGCode(`G1 X${f(positon.x)} Y${f(positon.y)}`);
    }
  };

  const [hoverPosition, setHoverPosition] = useState<Position | null>(null);
  const handlePointerMove = (e: React.PointerEvent<HTMLDivElement>) => {
    setHoverPosition(normalizePosition(e, config));
  };
  const handlePointerLeave = () => {
    setHoverPosition(null);
  };

  const pos = hoverPosition || toolPosition;

  const markerStyle = {
    left: `${nmap(toolPosition.x, config.minX, config.maxX, 0, 100)}%`,
    top: `${nmap(toolPosition.y, config.minY, config.maxY, 0, 100)}%`,
  };

  return (
    <div className={s.printSurface}>
      <div
        className={s.canvasWrapper}
        onPointerMove={handlePointerMove}
        onPointerLeave={handlePointerLeave}
        onClick={handleClick}
      >
        <Canvas width={width} height={height} />
        <div className={s.marker} style={markerStyle} />
      </div>
      <div className={s.controls}>
        <div className={s.position}>
          X: <span>{f(pos.x)}</span> Y: <span>{f(pos.y)}</span>
        </div>

        <Switch
          options={[
            { id: 'move', label: 'Move' },
            { id: 'line', label: 'Line' },
            { id: 'circle', label: 'Circle' },
          ]}
          activeId={mode}
          setActiveId={handleModeChange}
        />
      </div>
    </div>
  );
}
