import { useRef, useState } from 'preact/hooks';
import { PointerEvent } from 'preact/compat';

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

export type DrawMode = 'move' | 'free' | 'line' | 'circle';

export function PrintSurface({
  config,
  toolPosition,
  setToolPosition,
  gcode,
  addGCode,
}: PrintSurfaceProps) {
  const width = config.maxX - config.minX;
  const height = config.maxY - config.minY;

  const [mode, setMode] = useState<DrawMode>('move');
  const [circleCenter, setCircleCenter] = useState<Position | null>(null);

  const handlePointerDown = (e: PointerEvent<HTMLDivElement>) => {
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
    if (mode === 'circle') {
      if (!circleCenter) {
        setCircleCenter(positon);
      } else {
        if (!isToolLowered(gcode)) {
          addGCode([moveGCode(positon), 'M3']);
        }
        addGCode([
          `G2 X${f(positon.x)} Y${f(positon.y)} I${f(circleCenter.x)} J${f(circleCenter.y)}`,
          'M5',
        ]);
        setCircleCenter(null);
      }
    }
  };

  const [hoverPosition, setHoverPosition] = useState<Position | null>(null);
  const handlePointerMove = (e: PointerEvent<HTMLDivElement>) => {
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
    setCircleCenter(null);
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
          drawMode={mode}
          circleCenter={circleCenter}
          hoverPosition={hoverPosition}
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
            { id: 'free', label: 'Free' },
            { id: 'line', label: 'Line' },
            { id: 'circle', label: 'Circle' },
          ]}
          activeId={mode}
          setActiveId={id => setMode(id as DrawMode)}
        />
      </div>
    </div>
  );
}
