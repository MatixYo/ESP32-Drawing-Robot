import cn from 'clsx';

interface ButtonProps {
  onClick: () => void;
  label: string;
  disabled?: boolean;
  active?: boolean;
}

import s from './button.module.css';

export function Button({ label, active, ...props }: ButtonProps) {
  return (
    <button
      {...props}
      type="button"
      className={cn(s.button, active && s.active)}
    >
      {label}
    </button>
  );
}
