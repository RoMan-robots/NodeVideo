// @ts-ignore
import * as fs from 'fs';
import * as path from 'path';

const napi = require(path.join(__dirname, 'addons', 'screen_capture.node'));

let outputStream: fs.WriteStream | null = null;
let totalBytes = 0;
let frameCount = 0;
let lastWriteErrorShown = false;

function startStreaming() {
    console.log('Починаємо трансляцію відео...');
    napi.start((frame: any) => {
        const buffer = frame.data as Buffer;
        frameCount++;
        totalBytes += buffer.length;

        if (outputStream && !outputStream.writableEnded) {
            outputStream.write(buffer);
            console.log(`[Кадр ${frameCount}] Розмір: ${buffer.length} байт. Загалом записано: ${Math.round(totalBytes / 1024 / 1024)} MB`); 
            lastWriteErrorShown = false;
        } else {
            if (!lastWriteErrorShown) {
                console.error("⚠️ Потік закрито або не ініціалізовано");
                lastWriteErrorShown = true;
            }
        }
    });
}

function startRecording() {
    console.log('Починаємо запис у файл "output_video.raw"...');
    outputStream = fs.createWriteStream('output_video.raw');
    outputStream.on('open', () => {
        console.log('Потік для запису відкрито');
    });
    outputStream.on('error', (err) => {
        console.error('Помилка при записі:', err.message);
    });
}

setTimeout(() => {
    console.log('⏹️ Завершення запису...');
    if (outputStream && !outputStream.writableEnded) {
        outputStream.end(() => {
            console.log(`✅ Запис завершено. Загальна кількість кадрів: ${frameCount}, обсяг: ${Math.round(totalBytes / 1024 / 1024)} MB`);
        });
    }
}, 5000);

startRecording();
startStreaming();