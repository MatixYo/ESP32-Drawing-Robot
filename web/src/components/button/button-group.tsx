import s from './button.module.css';

interface ButtonGroupProps {
  children: React.ReactNode;
}

export function ButtonGroup({ children }: ButtonGroupProps) {
  return <div className={s.group}>{children}</div>;
}
