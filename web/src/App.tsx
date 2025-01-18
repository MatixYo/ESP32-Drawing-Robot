import { useState } from 'react';
import { Position } from './types/position';
import { Button } from './components/button/button';
import { ButtonGroup } from './components/button/button-group';
import { Card } from './components/card/card';
import { PrintSurface } from './components/print-surface/print-surface';
import { useQuery } from './hooks/use-query';
import {
  assembly,
  home,
  raiseTool,
  restart,
  print,
  getConfig,
} from './lib/queries';
import { useGCode } from './reducers/gcode-reducer';

export function App() {
  const [toolPosition, setToolPosition] = useState<Position>({ x: 0, y: 0 });
  const { gcode, clear: clearGCode } = useGCode();
  const config = useQuery(getConfig);

  return (
    <>
      <Card>
        {config.data && (
          <PrintSurface
            config={config.data}
            toolPosition={toolPosition}
            setToolPosition={setToolPosition}
          />
        )}
      </Card>

      <Card>
        <ButtonGroup>
          <Button label="Move pen" onClick={raiseTool} active />
          <Button label="Draw line" onClick={raiseTool} />
          <Button label="Clear GCode" onClick={clearGCode} />
          <Button label="Draw line" onClick={raiseTool} />
          <Button label="Print" onClick={() => print(gcode)} />
        </ButtonGroup>
      </Card>

      <Card title="Controls">
        <ButtonGroup>
          <Button label="Raise pen" onClick={raiseTool} />
          <Button label="Lower pen" onClick={() => raiseTool(false)} />
          <Button label="Home pen" onClick={home} />
          <Button label="Restart ESP" onClick={restart} />
          <Button label="Assembly" onClick={assembly} />
        </ButtonGroup>
      </Card>

      {gcode.length > 0 && (
        <Card title="GCode">
          <pre>{gcode.join('\n')}</pre>
        </Card>
      )}
    </>
  );
}
