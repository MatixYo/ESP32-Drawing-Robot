import { Button } from './button';

import s from './button.module.css';

interface SwitchOption<T extends string> {
  id: T;
  label: string;
}

interface SwitchProps<T extends string> {
  options: SwitchOption<T>[];
  activeId: T;
  setActiveId: (id: T) => void;
}

export function Switch<T extends string>({
  options,
  activeId,
  setActiveId,
}: SwitchProps<T>) {
  return (
    <div className={s.switch}>
      {options.map(option => (
        <Button
          key={option.id}
          label={option.label}
          active={option.id === activeId}
          onClick={() => setActiveId(option.id)}
        />
      ))}
    </div>
  );
}
