import s from './print-surface.module.css';

const SIZE_MULTIPLIER = 10;

interface CanvasProps {
  width: number;
  height: number;
}

export function Canvas({ width, height }: CanvasProps) {
  return (
    <canvas
      width={width * SIZE_MULTIPLIER}
      height={height * SIZE_MULTIPLIER}
      className={s.canvas}
    />
  );
}
