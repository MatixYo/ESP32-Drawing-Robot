import { useRef, useState } from 'preact/hooks';

import { nmap, normalizePosition } from '../../lib/helpers';
import { Config } from '../../types/config';
import { Position } from '../../types/position';
import { Canvas } from './canvas';
import s from './print-surface.module.css';
import { Switch } from '../button/switch';
import { isToolLowered, moveGCode, f } from '../../lib/gcode';
import { useOutsideClick } from '../../hooks/use-outside-click';

interface PrintSurfaceProps {
  config: Config;
  toolPosition: Position;
  setToolPosition: (position: Position) => void;
  gcode: string[];
  addGCode: (line: string | string[]) => void;
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

  const handleClick = (e: React.PointerEvent<HTMLDivElement>) => {
    const positon = normalizePosition(e, config);

    if (mode === 'move') {
      setToolPosition(positon);
    }
    if (mode === 'line') {
      if (!isToolLowered(gcode)) {
        addGCode([moveGCode(positon), 'M3']);
      }
      addGCode(moveGCode(positon));
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
        onClick={handleClick}
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
            // { id: 'circle', label: 'Circle' },
          ]}
          activeId={mode}
          setActiveId={setMode}
        />
      </div>
    </div>
  );
}
