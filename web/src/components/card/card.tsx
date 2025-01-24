import { useState } from 'preact/hooks';
import { Button } from '../button/button';
import s from './card.module.css';
import { ReactNode } from 'preact/compat';

interface CardProps {
  title?: ReactNode;
  children: ReactNode;
  expandable?: boolean;
}

export function Card({ title, children, expandable }: CardProps) {
  const [expanded, setExpanded] = useState(false);
  const toggle = () => setExpanded(!expanded);

  return (
    <div className={s.card}>
      {!!title && (
        <h2 className={s.title}>
          {title}
          {expandable && (
            <Button label={expanded ? 'Hide' : 'Show'} onClick={toggle} />
          )}
        </h2>
      )}
      {(!expandable || expanded) && children}
    </div>
  );
}
