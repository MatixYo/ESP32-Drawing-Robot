import { useRef, useState } from 'preact/hooks';

import { calculateDistance, nmap, normalizePosition } from '../../lib/helpers';
import { Config } from '../../types/config';
import { Position } from '../../types/position';
import { Canvas } from './canvas';
import s from './print-surface.module.css';
import { Switch } from '../button/switch';
import { isToolLowered, moveGCode, f, parseGCodeLine } from '../../lib/gcode';
import { useOutsideClick } from '../../hooks/use-outside-click';

const FREE_MOVE_THRESHOLD = 1;

interface PrintSurfaceProps {
  config: Config;
  toolPosition: Position;
  setToolPosition: (position: Position) => void;
  gcode: string[];
  addGCode: (line: string | string[]) => void;
}

type Mode = 'move' | 'line' | 'free';

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

  const handlePointerDown = (e: React.PointerEvent<HTMLDivElement>) => {
    const positon = normalizePosition(e, config);

    if (mode === 'move') {
      setToolPosition(positon);
    }
    if (['line', 'free'].includes(mode)) {
      if (!isToolLowered(gcode)) {
        addGCode([moveGCode(positon), 'M3']);
      }
      addGCode(moveGCode(positon));
    }
  };

  const [hoverPosition, setHoverPosition] = useState<Position | null>(null);
  const handlePointerMove = (e: React.PointerEvent<HTMLDivElement>) => {
    const normalized = normalizePosition(e, config);
    setHoverPosition(normalized);

    if (mode === 'free' && e.buttons === 1) {
      const last = parseGCodeLine(gcode.at(-1) || '');

      if (last?.cmd === 'G1') {
        const lastPosition = { x: last.params['X'], y: last.params['Y'] };
        if (calculateDistance(lastPosition, normalized) < FREE_MOVE_THRESHOLD)
          return;
      }

      addGCode(moveGCode(normalized));
    }
  };
  const handlePointerLeave = () => {
    setHoverPosition(null);
  };

  const handlePointerUp = () => {
    if (mode === 'free' && isToolLowered(gcode)) {
      addGCode('M5');
    }
  };

  const pos = hoverPosition || toolPosition;

  const markerStyle = {
    left: `${nmap(toolPosition.x, config.minX, config.maxX, 0, 100)}%`,
    top: `${nmap(toolPosition.y, config.minY, config.maxY, 0, 100)}%`,
  };

  const containerRef = useRef<HTMLDivElement>(null);
  useOutsideClick(containerRef, () => {
    if (isToolLowered(gcode)) {
      addGCode('M5');
    }
  });

  return (
    <div className={s.printSurface}>
      <div
        className={s.canvasWrapper}
        onPointerMove={handlePointerMove}
        onPointerLeave={handlePointerLeave}
        onPointerUp={handlePointerUp}
        onPointerDown={handlePointerDown}
        ref={containerRef}
      >
        <Canvas
          width={width}
          height={height}
          gcode={gcode}
          config={config}
          hoverPosition={(mode === 'line' && hoverPosition) || null}
        />
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
            { id: 'free', label: 'Free' },
            // { id: 'circle', label: 'Circle' },
          ]}
          activeId={mode}
          setActiveId={setMode}
        />
      </div>
    </div>
  );
}
