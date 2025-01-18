interface ButtonGroupProps {
  children: React.ReactNode;
}

import s from './button.module.css';

export function ButtonGroup({ children }: ButtonGroupProps) {
  return <div className={s.group}>{children}</div>;
}
