import { useState } from 'react';
import { f, nmap } from '../../lib/helpers';
import { Config } from '../../types/config';
import { Position } from '../../types/position';
import { Canvas } from './canvas';
import s from './print-surface.module.css';

interface PrintSurfaceProps {
  config: Config;
  toolPosition: Position;
  setToolPosition: (position: Position) => void;
}

function normalizePosition(e: React.PointerEvent, config: Config): Position {
  const rect = e.currentTarget.getBoundingClientRect();
  const x = nmap(
    e.clientX - rect.left,
    0,
    rect.width,
    config.minX,
    config.maxX,
  );
  const y = nmap(
    e.clientY - rect.top,
    0,
    rect.height,
    config.minY,
    config.maxY,
  );

  return { x, y };
}

export function PrintSurface({
  config,
  toolPosition,
  setToolPosition,
}: PrintSurfaceProps) {
  const width = config.maxX - config.minX;
  const height = config.maxY - config.minY;

  const handleClick = (e: React.PointerEvent<HTMLDivElement>) => {
    const positon = normalizePosition(e, config);
    setToolPosition(positon);
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
        <div>
          X: {f(pos.x)}, Y: {f(pos.y)}
        </div>
        <div></div>
      </div>
    </div>
  );
}
