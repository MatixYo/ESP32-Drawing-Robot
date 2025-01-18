import { useEffect, useRef } from 'preact/hooks';
import s from './print-surface.module.css';
import { parseGCodeLine, moveGCode, circleGCode } from '../../lib/gcode';
import { calculateBearing, calculateDistance, nmap } from '../../lib/helpers';
import { Config } from '../../types/config';
import { DrawMode } from './print-surface';
import { Position } from '../../types/position';

const SIZE_MULTIPLIER = 10;
const LINE_WIDTH = 5;

interface CanvasProps {
  gcode: string[];
  config: Config;
  width: number;
  height: number;
  drawMode: DrawMode;
  hoverPosition: Position | null;
  circleCenter: Position | null;
}

export function Canvas({
  gcode,
  config: c,
  width,
  height,
  drawMode,
  hoverPosition,
  circleCenter,
}: CanvasProps) {
  const ref = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const ctx = ref.current?.getContext('2d');
    if (!ref.current || !ctx) return;

    ctx.reset();
    ctx.clearRect(0, 0, ref.current.width, ref.current.height);
    ctx.lineWidth = LINE_WIDTH;
    ctx.lineCap = 'round';
    ctx.strokeStyle = 'black';

    const drawnGcode = [...gcode];
    if (hoverPosition) {
      if (drawMode === 'line') {
        drawnGcode.push(moveGCode(hoverPosition));
      } else if (drawMode === 'circle' && circleCenter) {
        drawnGcode.push(circleGCode(hoverPosition, circleCenter));
      }
    }

    let isDrawing = false;

    for (const line of drawnGcode) {
      const { cmd, params: p } = parseGCodeLine(line);

      if (cmd === 'G1') {
        const x = nmap(p['X'], c.minX, c.maxX, 0, ref.current.width);
        const y = nmap(p['Y'], c.minY, c.maxY, 0, ref.current.height);

        if (isDrawing) {
          ctx.lineTo(x, y);
        } else {
          ctx.beginPath();
          ctx.moveTo(x, y);
        }
      } else if (cmd === 'G2') {
        const x = nmap(p['X'], c.minX, c.maxX, 0, ref.current.width);
        const y = nmap(p['Y'], c.minY, c.maxY, 0, ref.current.height);
        const i = nmap(p['I'], c.minX, c.maxX, 0, ref.current.width);
        const j = nmap(p['J'], c.minY, c.maxY, 0, ref.current.height);

        if (isDrawing || hoverPosition) {
          const radius = calculateDistance({ x: i, y: j }, { x, y });
          const startAngle = calculateBearing({ x: i, y: j }, { x, y });

          ctx.moveTo(x, y);
          ctx.arc(i, j, radius, startAngle, startAngle + 2 * Math.PI);
        }
      } else if (cmd === 'M3') {
        isDrawing = true;
        ctx.beginPath();
      } else if (cmd === 'M5') {
        isDrawing = false;
        ctx.stroke();
      }
    }

    ctx.stroke();
  }, [gcode, c, ref, drawMode, circleCenter, hoverPosition]);

  return (
    <canvas
      ref={ref}
      width={width * SIZE_MULTIPLIER}
      height={height * SIZE_MULTIPLIER}
      className={s.canvas}
    />
  );
}
