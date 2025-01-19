import { useState, useEffect } from 'preact/hooks';
import s from './slider.module.css';

interface SliderProps {
  value: number;
  onChange: (value: number) => void;
  min?: number;
  max?: number;
  label?: string;
  fn?: (value: number) => string;
}

export function Slider({
  value,
  onChange,
  min = 0,
  max = 100,
  label = '',
  fn = x => x.toString(),
}: SliderProps) {
  const [internalValue, setInternalValue] = useState(value);

  useEffect(() => {
    setInternalValue(value);
  }, [value]);

  const handlePointerUp = () => {
    onChange(internalValue);
  };

  return (
    <label className={s.sliderContainer}>
      {label && <span className={s.label}>{label}</span>}
      <div className={s.inputWrapper}>
        <input
          className={s.rangeInput}
          type="range"
          min={min}
          max={max}
          value={internalValue}
          onInput={e =>
            setInternalValue((e.target as HTMLInputElement).valueAsNumber)
          }
          onPointerUp={handlePointerUp}
        />
        <span className={s.valueDisplay}>{fn(internalValue)}</span>
      </div>
    </label>
  );
}
