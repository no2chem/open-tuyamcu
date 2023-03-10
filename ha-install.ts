#!/usr/bin/env -S node --no-warnings --loader ts-node/esm

/*SPDX-License-Identifier: MIT
Copyright (C) 2023 by Michael Wei                          
michael@wei.email

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

import yargs from 'yargs'
import { hideBin } from 'yargs/helpers'
import MQTT from 'async-mqtt';

const argv = await yargs(hideBin(process.argv))
    .help("Installs your device into homeassistant through MQTT Discovery")
        .option('broker', {
            alias: 'b',
            describe: "The MQTT broker to send discovery messages to",
            type: "string"
        })
        .option('broker-username', {
            describe: "What broker username to use",
            type: "string"
        })
        .option('broker-password', {
            describe: "What broker password to use",
            type: "string"
        })
        .option('name', {
            alias: 'n',
            describe: "What to name the device in Home Assistant",
            type: "string"
        })
        .option('ha-topic', {
            describe: "The topic used for homeassistant discovery",
            type: "string",
            default: 'homeassistant'
        })
        .option('manufacturer', {
            describe: "What to use as the manufacturer name for the device node",
            type: "string",
            default: "Treatlife"
        })
        .option('model', {
            describe: "What to use as the model name for the device node",
            type: "string",
            default: "DS02S"
        })
        .option('device-version', {
            describe: "What to use as the version for the device node",
            type: "string",
            default: "1.0.0"
        })
        .option('identifier', {
            alias: 'i',
            describe: "The identifier to use. Defaults to lower snake case of name.",
            type: "string" 
        })
        .option('device-topic', {
            alias: 'd',
            describe: "The 'client topic' configured in OpenBK",
            type: "string"
        })
        .demandOption(['broker', 'name', 'device-topic'], 'Need at least a broker, name and device topic to install')
.parse();

const mqtt = await MQTT.connectAsync(argv.broker, { password: argv.brokerPassword, username: argv.brokerUsername });

const deviceIdentifier = argv.identifier ? argv.identifier : (argv.name as string).toLowerCase().replaceAll(' ', '_');
const deviceNode = {
    "identifiers" : [
        deviceIdentifier
    ],
    "name": argv.name,
    "sw_version" : argv.deviceVersion,
    "model": argv.model,
    "manufacturer": argv.manufacturer
}

interface AutomationDef {
    name : string;
    type: string;
    topic: string;
}

const buttons : AutomationDef[] = [
    {
        name: "Main Button",
        type: "button_main",
        topic: "0"
    },
    {
        name: "Plus Button",
        type: "button_plus",
        topic: "1"
    },
    {
        name: "Minus Button",
        type: "button_minus",
        topic: "2"
    },
]

for (const button of buttons) {
    const discoveryJson = {
        "automation_type" : "trigger",
        "type": "button_short_press",
        "subtype": button.type,
        "payload": "1",
        "topic": `${argv['device-topic']}/${button.topic}/get`,
        "device" : deviceNode,
        "unique_id": `${deviceNode.identifiers[0]}_${button.type}`,
        "name": `${deviceNode.name} ${button.name}`
    }

   await mqtt.publish(`${argv.haTopic}/device_automation/${deviceIdentifier}/${button.type}/config`, JSON.stringify(discoveryJson), { retain : true });

   // For each button, also publish state as binary sesnor

   const discoverySensorJson = {
        "payload_on": "1",
        "payload_off": "0",
        "~": argv.deviceTopic, 
        "availability_topic" : `~/connected`,
        "state_topic": `~/${button.topic}/get`,
        "device" : deviceNode,
        "unique_id": `${deviceNode.identifiers[0]}_${button.type}_sensor`,
        "name": `${deviceNode.name} ${button.name}`
    }

    await mqtt.publish(`${argv.haTopic}/binary_sensor/${deviceIdentifier}/${button.type}/config`, JSON.stringify(discoverySensorJson), { retain : true });
}

// special automation for connected
await mqtt.publish(`${argv.haTopic}/device_automation/${deviceIdentifier}/connected/config`, JSON.stringify(
    {
        "automation_type" : "trigger",
        "type": "online",
        "subtype": "connection",
        "payload": "online",
        "topic": `${argv['device-topic']}/connected`,
        "device" : deviceNode,
        "unique_id": `${deviceNode.identifiers[0]}_connected`,
        "name": `${deviceNode.name} connected`
    }
), { retain : true });




interface SwitchDef {
    name : string;
    topic: string;
}

const switches : SwitchDef[] = [
    {
        name: "LED 0",
        topic: "10"
    },
    {
        name: "LED 1",
        topic: "11"
    },
    {
        name: "LED 2",
        topic: "12"
    },
    {
        name: "LED 3",
        topic: "13"
    },
    {
        name: "LED 4",
        topic: "14"
    },
    {
        name: "LED 5",
        topic: "15"
    },
    {
        name: "LED 6",
        topic: "16"
    },
    {
        name: "LED Main White",
        topic: "17"
    },
    {
        name: "LED Main Red",
        topic: "18"
    },
    {
        name: "Relay",
        topic: "32"
    },
    {
        name: "Force Triac On",
        topic: "42"
    },
]


for (const sw of switches) {
    const discoveryJson = {
        "payload_on": "1",
        "payload_off": "0",
        "~": argv.deviceTopic,
        "state_topic": `~/${sw.topic}/get`,
        "command_topic": `~/${sw.topic}/set`,
        "availability_topic" : `~/connected`,
        "qos": 1,
        "device" : deviceNode,
        "unique_id": `${deviceNode.identifiers[0]}_${sw.name.toLowerCase().replaceAll(' ', '_')}`,
        "name": `${deviceNode.name} ${sw.name}`
    }

    await mqtt.publish(`${argv.haTopic}/switch/${deviceIdentifier}/${discoveryJson.unique_id}/config`, JSON.stringify(discoveryJson), { retain : true });
}

interface NumberDef {
    name : string;
    topic: string;
    min : number;
    max : number;
}

const numbers : NumberDef[] = [
    {
        name: "LED Flags",
        topic: "20",
        min: 0,
        max: 511
    }
]

for (const nums of numbers) {
    const discoveryJson = {
        "~": argv.deviceTopic,
        "state_topic": `~/${nums.topic}/get`,
        "command_topic": `~/${nums.topic}/set`,
        "availability_topic" : `~/connected`,
        "qos": 1,
        "device" : deviceNode,
        "unique_id": `${deviceNode.identifiers[0]}_${nums.name.toLowerCase().replaceAll(' ', '_')}`,
        "name": `${deviceNode.name} ${nums.name}`,
        "max" : nums.max,
        "min" : nums.min,
        "mode" : "box"
    }

    await mqtt.publish(`${argv.haTopic}/number/${deviceIdentifier}/${discoveryJson.unique_id}/config`, JSON.stringify(discoveryJson), { retain : true });
}

await mqtt.end();
console.log(`Done with discovery for device ${argv.name} to broker ${argv.broker}`);
