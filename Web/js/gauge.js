/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*global jQuery */
(function($) {

    $.fn.percentageLoader = function(params) {

        var settings, outerDiv, canvas, ctx, cX, cY,
                percentageText, valueText, items, i, item, selectors, s, progress,
                value, lingrad, innerGrad, tubeGrad, innerRadius, innerBarRadius, outerBarRadius,
                radius, startAngle, endAngle, counterClockwise, completeAngle, setProgress, setValue,
                applyAngle, drawLoader, clipValue;

        /* Specify default settings */
        settings = {
            width: 256,
            height: 256,
            progress: 0,
            min: 0,
            max: 100,
            textcolor: '#798243',
            value: '0kb',
            controllable: false
        };

        /* Override default settings with provided params, if any */
        if (params !== undefined) {
            $.extend(settings, params);
        } else {
            params = settings;
        }

        outerDiv = document.createElement('div');
        outerDiv.style.width = settings.width + 'px';
        outerDiv.style.height = settings.height + 'px';
        outerDiv.style.position = 'relative';

        $(this).append(outerDiv);

        /* Create our canvas object */
        canvas = document.createElement('canvas');
        canvas.setAttribute('width', settings.width);
        canvas.setAttribute('height', settings.height);
        outerDiv.appendChild(canvas);


        ctx = canvas.getContext("2d");

        //dimensions
        cX = canvas.width / 2 - 1;
        cY = canvas.height / 2 - 1;

        /* The inner circle is 2/3rds the size of the outer one */
        innerRadius = cX - 26;
        /* Outer radius is the same as the width / 2, same as the centre x
         * (but we leave a little room so the borders aren't truncated) */
        radius = cX - 24;
        //Variables
        var degrees = 0;
        var new_degrees = 0;
        var difference = 0;
        var color = "lightgreen"; //green looks better to me
        var bgcolor = "#222";
        var text;
        var animation_loop, redraw_loop;
        var radiusMeter = cX - 9;
        function init()
        {
            /* Clear canvas entirely */
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ;

            /*            //Background 360 degree arc
             ctx.beginPath();
             ctx.strokeStyle = bgcolor;
             ctx.lineWidth = 5;
             ctx.arc(cX, cY, radius, 0, Math.PI * 2, false); //you can see the arc now
             ctx.stroke();
             */
            //gauge will be a simple arc
            //Angle in radians = angle in degrees * PI / 180
            var radians = degrees * (Math.PI / 180);
            var start_radian = (360 - 210) * (Math.PI / 180);
            var end_radian = (360 - 300) * (Math.PI / 180);
            radians = radians - start_radian - end_radian;
            $("#grad").html(degrees + "/" + radians);
            ctx.beginPath();
            ctx.strokeStyle = color;
            ctx.lineWidth = 5;
            //The arc starts from the rightmost end. If we deduct 90 degrees from the angles
            //the arc will start from the topmost end
            ctx.arc(cX, cY, radius, start_radian, radians, false
                    );

            //you can see the arc now
            ctx.stroke();

            //Lets add the text
            ctx.fillStyle = settings.textcolor;
            ctx.font = "15px bebas";
            text = Math.floor(degrees / 240 * 100) + "%";
            //Lets center the text
            //deducting half of text width from position x
            var text_width = ctx.measureText(text).width;
            //adding manual value to position y since the height of the text cannot
            //be measured easily. There are hacks but we will keep it manual for now.
            ctx.fillText(text, cX - text_width / 2, cY + 25);
        }

        var speed = 0;
        var largeHashCount = 10;  // The spacing of large hashes, 16 per 360 degrees
        var smallHashCount = 72;  // The spacing of small hashes, 80 per 360 degrees
        var outterRadius = radiusMeter - 5;    // How far from the center both large and small hashes will start
        var largeHashRadius = outterRadius - 7; // How far from the center large hashes will end
        var smallHashRadius = outterRadius - 5; // How far from the center small hashes will end

        // Get the outter points of our large hash lines
        var largeHashesOut = getNPointsOnCircle(cX, cY, outterRadius, largeHashCount);
        // Get the inner pints ouf our large hash li
        var largeHashesIn = getNPointsOnCircle(cX, cY, largeHashRadius, largeHashCount);
        var ScaleTextOut = getNPointsOnCircle(cX, cY, radiusMeter + 2, largeHashCount);

        // Get the outter points of our small hashes
        var smallHashesOut = getNPointsOnCircle(cX, cY, outterRadius, smallHashCount);
        // Get the inner points out of our small hashes
        var smallHashesIn = getNPointsOnCircle(cX, cY, smallHashRadius, smallHashCount);

        // Get the outter points of our needle, use 160 steps instead of a full 360
        var needleHashes = getNPointsOnCircle(cX, cY, outterRadius - 5, 240);


        function getNPointsOnCircle(x, y, radius, n) {
            // Found out the spacing of each point based on n points in 360 degrees
            var hashSpacing = /*Math.PI * 2*/ 240 * Math.PI / 180 / n;
            var points = [];

            // For each point of n, find it's position based on the given radius and starting x, y
            for (var i = 0; i <= n; i++) {
                var a = hashSpacing * i + (150 * Math.PI / 180);
                points.push({x: x + Math.cos(a) * radius, y: y + Math.sin(a) * radius});
            }

            return points;
        }


        function drawMeter() {
            // Outside border
            ctx.strokeStyle = "#000000";
            ctx.lineWidth = 4;
            ctx.beginPath();
            ctx.arc(cX, cY, radiusMeter - 5, 0, Math.PI * 2, false);
            ctx.closePath();
            ctx.stroke();

            // Small Hashes
            ctx.strokeStyle = "#FFF";
            ctx.lineWidth = 1;
            for (var i = 0; i < smallHashesOut.length; i++) {
                ctx.beginPath();
                ctx.moveTo(smallHashesOut[i].x, smallHashesOut[i].y);
                ctx.lineTo(smallHashesIn[i].x, smallHashesIn[i].y);
                ctx.closePath();
                ctx.stroke();
            }

            // Large Hashes
            ctx.strokeStyle = "#00ff00";
            ctx.lineWidth = 2;
            for (var i = 0; i < largeHashesOut.length; i++) {
                ctx.beginPath();
                ctx.moveTo(largeHashesOut[i].x, largeHashesOut[i].y);
                ctx.lineTo(largeHashesIn[i].x, largeHashesIn[i].y);
                ctx.closePath();
                ctx.stroke();

                //Lets add the text
                ctx.fillStyle = color;
                ctx.font = "10px bebas";
                text = (settings.max - settings.min) / i;
                //Lets center the text
                //deducting half of text width from position x
                var text_width = ctx.measureText(text).width;
                //adding manual value to position y since the height of the text cannot
                //be measured easily. There are hacks but we will keep it manual for now.
                ctx.fillText(text, ScaleTextOut[i].x - text_width / 2, ScaleTextOut[i].y);
            }
        }
        function drawNeedle() {
            // Needle Base
            ctx.fillStyle = "#000000";
            ctx.beginPath();
            ctx.arc(cX, cY, 5, 0, Math.PI * 2, false);
            ctx.closePath();
            ctx.fill();

            // Needle
            ctx.strokeStyle = "#FF0000";
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.moveTo(cX, cY);
            ctx.lineTo(needleHashes[degrees].x, needleHashes[degrees].y);
            ctx.closePath();
            ctx.stroke();
        }

        function draw()
        {
            //Cancel any movement animation if a new chart is requested
            if (typeof animation_loop !== undefined)
                clearInterval(animation_loop);

            //random degree from 0 to 360
            proz = Math.round(Math.random() * 100);
            proz = 100;
            new_degrees = 240 * (proz / 100);
            $("#grad").html(proz);
            //new_degrees = 240;
            difference = new_degrees - degrees;
            //This will animate the gauge to new positions
            //The animation will take 1 second
            //time for each frame is 1sec / difference in degrees
            animation_loop = setInterval(animate_to, 1000 / difference);
        }

        //function to make the chart move to new degrees
        function animate_to()
        {
            //clear animation loop if degrees reaches to new_degrees
            if (degrees === new_degrees)
                clearInterval(animation_loop);

            if (degrees < new_degrees)
                degrees++;
            else
                degrees--;

            init();
            drawMeter();
            drawNeedle();
        }

        //Lets add some animation for fun
        draw();
        
       
  

        return this;

    };
    $.fn.percentageLoader ={
       
        
        myValue: function(value) {

            if (typeof(value) !== "undefined") {
                 new_degrees = 240 * (value / 100);
            
            //new_degrees = 240;
            difference = new_degrees - degrees;
            //This will animate the gauge to new positions
            //The animation will take 1 second
            //time for each frame is 1sec / difference in degrees
            animation_loop = setInterval(animate_to, 1000 / difference);
            
            } else {
                return myValue;
            }

        }
        }
        
}(jQuery));

