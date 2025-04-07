// @ts-ignore
import * as fs from 'fs';
import * as path from 'path';

const napi = require(path.join(__dirname, 'addons', 'screen_capture.node'));

let outputStream: fs.WriteStream | null = null;

function startStreaming() {
    console.log('Починаємо трансляцію відео...');
    napi.start((frame: any) => {
        const buffer = frame.data as Buffer;
        console.log('Отримано новий кадр:', buffer.length, 'байт');
        if (outputStream) {
            outputStream.write(buffer);
        }
    });
}

function startRecording() {
    outputStream = fs.createWriteStream('output_video.raw');
    console.log('Запис почався...');
    if (!outputStream) {
        console.error("Не вдалося створити потік для запису.");
    }
}

setTimeout(() => {
    console.log('Запис завершено');
    if (outputStream) {
        outputStream.end();
    }
}, 5000); 

startRecording();
startStreaming();