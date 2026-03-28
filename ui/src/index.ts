import { AST } from '../lib/lang-maker/ast';
import * as grammar from "../lib/grammar";

import { Engine } from '../lib/kernel';
import { LedBarDevice } from './devices/ledbar.device'
import { LedStripDevice } from './devices/ledstrip.device'
import { Sling2DDevice } from './devices/sling2d-device';

import Goo from '@quimblos/goo';
import { GooRouter } from '@quimblos/goo/src/router'

import '@quimblos/goo/components/goo-style.goo'
import '@quimblos/goo/components/goo-app.goo'
import '@quimblos/goo/components/goo-page-error.goo'
import '@quimblos/goo/components/goo-card.goo'
import '@quimblos/goo/components/goo-btn.goo'
import '@quimblos/goo/components/goo-list.goo'
import '@quimblos/goo/components/goo-modal.goo'
import '@quimblos/goo/components/goo-form.goo'
import '@quimblos/goo/components/goo-chart.goo'

import './components/qb-console.goo'
import './components/qb-editor.goo'
import './components/qb-servo.goo'
import './components/qb-water-tank.goo'
import './pages/page-welcome.goo'
import './pages/page-devices.goo'
import './pages/page-sandbox.goo'
import './my-app.goo'

async function setup() {

  // Quimblos Engine

  const engine = await Engine.init();
  engine.put_device(new LedBarDevice());
  engine.put_device(new Sling2DDevice());
  engine.put_device(new LedStripDevice());
  
  const ast = new AST(grammar);
  (window as any).ast = ast;

  // Goo Routes

  const routes = GooRouter.tree('my-app', $ => $
    .alias('Home')
    .menu($ => [$('Home')])
    .child('welcome', $ => $
      .slot('my-app|page', 'page-welcome')
    )
    .child('devices', $ => $
      .alias('Devices')
      .menu($ => [$('Devices')])
      .slot('my-app|page', 'page-devices')
    )
    .child('sandbox', $ => $
      .alias('Sandbox')
      .menu($ => [$('Sandbox')])
      .slot('my-app|page', 'page-sandbox')
    )
    .follow_to('welcome')
  )

  // Goo Components

  Goo.init([
    'goo-style',
    'my-app'
  ], routes);

}

void setup();