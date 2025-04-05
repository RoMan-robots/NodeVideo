
// @ts-ignore
import * as fs from 'fs';
// @ts-ignore
import * as napi from './screen_capture.node'; 

let outputStream: fs.WriteStream | null = null;

function startStreaming() {
    console.log('Починаємо трансляцію відео...');
    napi.start((frame: any) => {
        const buffer = frame.data as Buffer;

        if (outputStream) {
            outputStream.write(buffer);
        }
    });
}

// Записуємо відео в файл
function startRecording() {
    outputStream = fs.createWriteStream('output_video.raw'); // Відео буде збережено в raw форматі
    console.log('Запис почався...');
}

// Зупиняємо запис через 5 секунд
setTimeout(() => {
    console.log('Запис завершено');
    if (outputStream) {
        outputStream.end();
    }
}, 5000);

// Починаємо захоплення та запис
startRecording();
startStreaming();
