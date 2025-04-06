import screen from 'screen_capture';

screen.startStream((frame) => {
    console.log(frame);
});

setTimeout(() => {
    screen.stopStream();
}, 5000);