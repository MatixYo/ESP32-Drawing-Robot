import { useEffect, useState } from 'preact/hooks';
import { Position } from './types/position';
import { Button } from './components/button/button';
import { ButtonGroup } from './components/button/button-group';
import { Card } from './components/card/card';
import { Status } from './components/status/status';
import { PrintSurface } from './components/print-surface/print-surface';
import { useQuery } from './hooks/use-query';
import {
  assembly,
  home,
  move,
  raiseTool,
  restart,
  getConfig,
  print,
} from './lib/queries';
import { useGCode } from './reducers/gcode-reducer';
import { Config } from './types/config';
import { handleStopPropagation, downloadFile, readFile } from './lib/helpers';

const initialConfig: Config = {
  minX: -50,
  minY: 25,
  maxX: 50,
  maxY: 125,
  homeX: 0,
  homeY: 25,
  speed: 150,
};

export function App() {
  const [toolPosition, setToolPosition] = useState<Position>({ x: 0, y: 0 });
  const { gcode, setGCode, addLine, clearAll, clearLine } = useGCode();
  const config = useQuery(getConfig, { initialData: initialConfig });

  useEffect(() => {
    if (config.data)
      setToolPosition({ x: config.data.homeX, y: config.data.homeY });
  }, [config.data]);

  const handleHome = () => {
    home();
    if (config.data) {
      setToolPosition({ x: config.data.homeX, y: config.data.homeY });
    }
  };

  const handleSetPosition = (position: Position) => {
    move(position);
    setToolPosition(position);
  };

  const handleSave = () => {
    downloadFile('drawing.gcode', gcode.join('\n'));
  };

  const handleLoad = async () => {
    const content = await readFile(['gcode', 'txt']);
    const lines = content.split('\n');
    setGCode(lines);
  };

  const hasLines = gcode.length > 0;

  return (
    <main>
      <Card
        title={
          <>
            Drawing Robot
            <Status />
          </>
        }
      >
        {config.data && (
          <PrintSurface
            config={config.data}
            toolPosition={toolPosition}
            setToolPosition={handleSetPosition}
            gcode={gcode}
            addGCode={addLine}
          />
        )}

        <hr />

        <ButtonGroup>
          <Button
            label="Clear"
            disabled={!hasLines}
            onClick={handleStopPropagation(clearAll)}
          />
          <Button
            label="Back"
            disabled={!hasLines}
            onClick={handleStopPropagation(clearLine)}
          />

          <Button
            label="Load"
            style={{ marginLeft: 'auto' }}
            onClick={handleLoad}
          />
          <Button label="Save" disabled={!hasLines} onClick={handleSave} />
          <Button
            label="Print"
            disabled={!hasLines}
            onClick={() => print(gcode)}
          />
        </ButtonGroup>
      </Card>

      <Card title="Control">
        <ButtonGroup>
          <Button label="Raise pen" onClick={() => raiseTool()} />
          <Button label="Lower pen" onClick={() => raiseTool(false)} />
          <Button label="Home pen" onClick={handleHome} />
          <Button label="Restart ESP" onClick={restart} />
          <Button label="Assembly" onClick={assembly} />
        </ButtonGroup>
      </Card>

      {gcode.length > 0 && (
        <Card title="GCode">
          <pre>{gcode.join('\n')}</pre>
        </Card>
      )}
    </main>
  );
}
