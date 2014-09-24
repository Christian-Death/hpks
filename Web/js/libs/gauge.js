/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*global jQuery */

(function($) {
    /* Strict mode for this plugin */
    "use strict";

    $.fn.percentageLoader = function(params) {
        var settings, outerDiv, canvas, ctx, cX, cY,
                
                percentageText, valueText, items, i, item, selectors, s, progress,
            value,  lingrad, innerGrad, tubeGrad, innerRadius, innerBarRadius, outerBarRadius,
            radius, startAngle, endAngle, counterClockwise, completeAngle, setProgress, setValue,
            applyAngle, drawLoader, clipValue;

        /* Specify default settings */
        settings = {
            width: 256,
            height: 256,
            progress: 0,
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
        cX = canvas.width/2 - 1;
        cY = canvas.height/2 - 1;
        
        /* The inner circle is 2/3rds the size of the outer one */
        innerRadius = cX * 0.6666;
        /* Outer radius is the same as the width / 2, same as the centre x
        * (but we leave a little room so the borders aren't truncated) */
        radius = cX - 2;
        //Variables
        var degrees = 0;
        var new_degrees = 0;
        var difference = 0;
        var color = "lightgreen"; //green looks better to me
        var bgcolor = "#222";
        var text;
        var animation_loop, redraw_loop;

        function init()
        {
             /* Clear canvas entirely */
            ctx.clearRect(0, 0, canvas.width, canvas.height);;

            //Background 360 degree arc
            ctx.beginPath();
            ctx.strokeStyle = bgcolor;
            ctx.lineWidth = 10;
            ctx.arc(cX, cY, radius, 0, Math.PI * 2, false); //you can see the arc now
            ctx.stroke();

            //gauge will be a simple arc
            //Angle in radians = angle in degrees * PI / 180
            var radians = degrees * Math.PI / 180;
            ctx.beginPath();
            ctx.strokeStyle = color;
            ctx.lineWidth = 10;
            //The arc starts from the rightmost end. If we deduct 90 degrees from the angles
            //the arc will start from the topmost end
            ctx.arc(cX, cY, radius, 0 - 180 * Math.PI / 180, radians - 180 * Math.PI / 180, false);
            //you can see the arc now
            ctx.stroke();

            //Lets add the text
            ctx.fillStyle = color;
            ctx.font = "20px bebas";
            text = Math.floor(degrees / 360 * 100) + "%";
            //Lets center the text
            //deducting half of text width from position x
            var text_width = ctx.measureText(text).width;
            //adding manual value to position y since the height of the text cannot
            //be measured easily. There are hacks but we will keep it manual for now.
            ctx.fillText(text, cX - text_width / 2, cY + 15);
        }

var speed=0;
var largeHashCount = 16;  // The spacing of large hashes, 16 per 360 degrees
    var smallHashCount = 80;  // The spacing of small hashes, 80 per 360 degrees
    var outterRadius = 48;    // How far from the center both large and small hashes will start
    var largeHashRadius = 40; // How far from the center large hashes will end
    var smallHashRadius = 45; // How far from the center small hashes will end
            
    // Get the outter points of our large hash lines
    var largeHashesOut = getNPointsOnCircle( cX, cY, outterRadius, largeHashCount );
    // Get the inner pints ouf our large hash li
    var largeHashesIn = getNPointsOnCircle( cX, cY, largeHashRadius, largeHashCount );
    // The hashes start on the right side of the circle
    // and we need to strip off the bottom 3, so remove 3,4,5
    largeHashesOut.splice(3, 3);
    largeHashesIn.splice(3, 3);
    
    // Get the outter points of our small hashes
    var smallHashesOut = getNPointsOnCircle( cX, cY, outterRadius, smallHashCount );
    // Get the inner points out of our small hashes
    var smallHashesIn = getNPointsOnCircle( cX, cY, smallHashRadius, smallHashCount);   
    // The hashes start on the right side of the circle
    // and we need to strip off the bottom 20, so remove 10,11,12,13....
    smallHashesOut.splice(10, 20);
    smallHashesIn.splice(10, 20);  
  
    // Get the outter points of our needle, use 160 steps instead of a full 360
    var needleHashes = getNPointsOnCircle( cX, cY, outterRadius - 5, 160);
    // The needle points start on the right side of the circle,
    // so shift the array so that they start on the bottom left where our first hash is
    needleHashes.push.apply( needleHashes, needleHashes.splice( 0, 60 ) );
    // Remove the last 40 points so they needle stops at hte last hash
    needleHashes.splice(-40);
    
    function getNPointsOnCircle( x, y, radius, n) {
        // Found out the spacing of each point based on n points in 360 degrees
        var hashSpacing = Math.PI * 2 / n;
        var points = [];
        
        // For each point of n, find it's position based on the given radius and starting x, y
        for( var i = 0; i < n; i++ ){
            var a = hashSpacing * i;
            points.push( { x: x + Math.cos( a ) * radius, y: y + Math.sin( a ) * radius } );
        } 
        
        return points;
    }  


function drawMeter() {
        // Outside border
        ctx.strokeStyle = "#FF00000";
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.arc( cX, cY, cX - 1, Math.PI * .75, Math.PI * 2.25, false );
        ctx.closePath();
        ctx.stroke();        
        
        // Small Hashes
        ctx.strokeStyle = "#696969";
        ctx.lineWidth = 1;
        for( var i = 0; i < smallHashesOut.length; i++ ) {
            ctx.beginPath();
            ctx.moveTo( smallHashesOut[i].x, smallHashesOut[i].y );
            ctx.lineTo( smallHashesIn[i].x, smallHashesIn[i].y );
            ctx.closePath();
            ctx.stroke();
        }
        
        // Large Hashes
        ctx.strokeStyle = "#00ff00";
        ctx.lineWidth = 2;
        for( var i = 0; i < largeHashesOut.length; i++ ) {
            ctx.beginPath();
            ctx.moveTo( largeHashesOut[i].x, largeHashesOut[i].y );
            ctx.lineTo( largeHashesIn[i].x, largeHashesIn[i].y );
            ctx.closePath();
            ctx.stroke();
        }      
    }
       function drawNeedle() {        
        // Needle Base
        ctx.fillStyle = "#000000";
        ctx.beginPath();
        ctx.arc( cX, cY, 5, 0, Math.PI * 2, false );
        ctx.closePath();
        ctx.fill();
        
        // Needle
        ctx.strokeStyle = "#FF0000";
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo( cX, cY );
        ctx.lineTo( needleHashes[degrees].x, needleHashes[degrees].y );
        ctx.closePath();
        ctx.stroke();
    }
    
        function draw()
        {
            //Cancel any movement animation if a new chart is requested
            if (typeof animation_loop !== undefined)
                clearInterval(animation_loop);

            //random degree from 0 to 360
            new_degrees = Math.round(Math.random() * 360);
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
        redraw_loop = setInterval(draw, 2000); //Draw a new chart every 2 seconds



        return this;
    };
}(jQuery));
