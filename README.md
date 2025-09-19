<h1><center>📝 Imageify 🖼️</center></h1>

## Introduction

> **Imageify** is a tool that allows users to convert text into images or vice versa. This isn't an application like DALL-E or Midjourney, but rather a simple utility to hide text as images, as the pixel values of the image.


## Why?

> Because steganography!
>
> It's not obvious while looking at an image full of blobs of pixels that it may contain hidden text. Actually, in this case, the text isn't even hidden. The image is exactly a visual representation of the text.
>
> This is peak hiding in plain sight.


## How To Run?

> No installation is required. Find the latest release on the [Releases](https://github.com/AlexJMercer/Imageify/releases) page and download the zipped folder.
>
> Unzip the folder and run `Imageify.exe -h` in your terminal to see the help message.
>
> A few examples of how to use the commands:
>
> First create a .txt file with some text in it, e.g. `input.txt`
>
> - Convert text to images: <br>`Imageify.exe --encode input.txt --output encodedImage.png`
>
> - Convert images back to text: <br>`Imageify.exe --decode encodedImage.png --output outputFile.txt`
>
> - Show help message: <br>`Imageify.exe -h`

## Additionally...

> If you encounter:
> - Bugs
> - Weird behavior
> - Have feature requests
> - Want to contribute
>
> Please open an issue or a pull request on the [GitHub repository](https://github.com/AlexJMercer/Imageify).
> I welcome any contributions and feedback!

## License
> This project is licensed under the MIT License. See the [LICENSE](LICENSE.txt) file for details.