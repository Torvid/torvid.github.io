// Sets the position and size of an element
function SetElementRect(element, x, y, sizeX, sizeY)
{
    if (!element)
        return;
    element.style.position = 'fixed';
    element.style.left = x + 'px';
    element.style.top = y + 'px';
    // for images and text
    element.setAttribute('width', sizeX);
    element.setAttribute('height', sizeY);
    // for boxes
    element.style.width = sizeX;
    element.style.height = sizeY;
}

// Fits one rectangle inside another
function Fit(screenSizeX, screenSizeY, sizeX, sizeY)
{
    var imageAspectRatio = sizeX / sizeY;
    var screenAspectRatio = screenSizeX / screenSizeY;

    if (imageAspectRatio > screenAspectRatio)
    {
        sizeX = screenSizeX;
        sizeY = screenSizeX / imageAspectRatio;
    }
    else
    {
        sizeY = screenSizeY;
        sizeX = screenSizeY * imageAspectRatio;
    }

    return [screenSizeX / 2 - sizeX / 2, screenSizeY / 2 - sizeY / 2, sizeX, sizeY]
}

function getActualWidth()
{
    return window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth || document.body.offsetWidth;
}
function getActualHeight()
{
    return window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight || document.body.offsetHeight;
}

var lightbox = document.createElement("div")
var background = document.createElement("div")
var image = document.createElement("img")
var text = document.createElement("div")
text.style.padding = "10px";
var video = undefined;
var videoSource = undefined;


document.body.appendChild(lightbox);
lightbox.appendChild(background);
lightbox.appendChild(image);
lightbox.appendChild(text);
lightbox.style.visibility = "hidden";

//lightbox = document.getElementById("lightbox");
//image = document.getElementById("lightbox_image");
//text = document.getElementById("lightbox_text");
//background = document.getElementById("lightbox_background");

background.style.backgroundColor = "#000000cc";

function disableLightbox()
{
    lightbox.style.visibility = "hidden";
    image.src = "";
    videoSource.src = "";
    if (video)
        video.remove();
}
background.onclick = disableLightbox;
image.onclick = disableLightbox;
video.onclick = disableLightbox;

isVideo = false;

function enableLightbox(path, title, desc, source)
{
    text.innerHTML = "<h2>" + title + "</h2>\n";
    text.innerHTML += desc;
    text.innerHTML += "<br/>";
    if (source)
        text.innerHTML += "source: " + "<a href=\"" + source + "\">" + source + "</a>";
    lightbox.style.visibility = "visible";

    if (video)
        video.remove();

    if (path.endsWith(".mp4"))
    {
        video = document.createElement("video")
        videoSource = document.createElement("source")
        video.setAttribute("controls", "")
        video.setAttribute("autoplay", "")
        //video.setAttribute("muted", "")
        //video.setAttribute("preload", "metadata")
        //video.setAttribute("playsinline", "")
        video.setAttribute("loop", "")
        video.appendChild(videoSource);
        lightbox.appendChild(video);

        isVideo = true;
        //video.style.display = "block";
        image.visibility = "hidden";
        videoSource.src = "";
        videoSource.src = path;
        videoSource.type = "video/mp4";
        //video.play();
    }
    else
    {
        isVideo = false;
        //video.style.display = "none";
        image.visibility = "visible";
        image.src = "";
        image.src = path;
    }
    Update();
}
function reportWindowSize()
{
    Update();
}

window.onresize = reportWindowSize;
function Update()
{
	var textHeight = (text.clientHeight * 2.0)+25;
    SetElementRect(background, 0, 0, getActualWidth(), getActualHeight());
    background.style.width = "100%";
    background.style.height = "100%";
    var sourceWidth = 0;
    var sourceHeight = 0;
    if (isVideo)
    {
        sourceWidth = video.videoWidth;
        sourceHeight = video.videoHeight;
    }
    else
    {
        sourceWidth = image.naturalWidth;
        sourceHeight = image.naturalHeight;
    }
	
	if(getActualHeight() - textHeight <= 10)
	{
		requestAnimationFrame(Update);
		return;
	}
	
    var rect = Fit(getActualWidth(), getActualHeight() - textHeight, sourceWidth, sourceHeight);

    var x = rect[0];
    var y = rect[1];
    var w = rect[2];
    var h = rect[3];

    //var quarterHeight = h / 5;

    SetElementRect(image, x, y, w, h);
    SetElementRect(video, x, y, w, h);
	
    SetElementRect(text, 0, y+h, w, textHeight);

	requestAnimationFrame(Update);

}
requestAnimationFrame(Update);