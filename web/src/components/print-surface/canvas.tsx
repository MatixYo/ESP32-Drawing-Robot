import { useEffect, useRef } from 'react';
import s from './print-surface.module.css';
import { parseGCodeLine, moveGCode } from '../../lib/gcode';
import { nmap } from '../../lib/helpers';
import { Config } from '../../types/config';

const SIZE_MULTIPLIER = 10;
const LINE_WIDTH = 5;

interface CanvasProps {
  gcode: string[];
  config: Config;
  width: number;
  height: number;
  hoverPosition: { x: number; y: number } | null;
}

export function Canvas({
  gcode,
  config: c,
  width,
  height,
  hoverPosition,
}: CanvasProps) {
  const ref = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const ctx = ref.current?.getContext('2d');
    if (!ref.current || !ctx) return;

    ctx.clearRect(0, 0, ref.current.width, ref.current.height);
    ctx.lineWidth = LINE_WIDTH;
    ctx.lineCap = 'round';
    ctx.strokeStyle = 'black';

    if (gcode.length === 0) return;

    const drawnGcode = hoverPosition
      ? [...gcode, moveGCode(hoverPosition)]
      : gcode;

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
      } else if (cmd === 'M3') {
        isDrawing = true;
        ctx.beginPath();
      } else if (cmd === 'M5') {
        isDrawing = false;
        ctx.stroke();
      }
    }

    ctx.stroke();
  }, [gcode, c, ref, hoverPosition]);

  return (
    <canvas
      ref={ref}
      width={width * SIZE_MULTIPLIER}
      height={height * SIZE_MULTIPLIER}
      className={s.canvas}
    />
  );
}
