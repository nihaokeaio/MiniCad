# miniCad Project

miniCad is a simple C++ drawing application that allows users to create and manipulate basic shapes on a canvas. This project serves as a mini CAD tool for educational purposes and demonstrates the use of object-oriented programming principles in C++.

## Features

- **Canvas**: A drawable area where shapes can be rendered and cleared.
- **Shapes**: Includes basic shapes like circles and rectangles, each with methods to draw themselves and set their position.
- **Utility Functions**: Functions to calculate the area and perimeter of shapes.

## Directory Structure

```
miniCad
├── include
│   ├── canvas.h       // Header file for the Canvas class
│   ├── shapes.h       // Header file for shape classes (Circle, Rectangle)
│   └── utils.h        // Header file for utility functions
├── src
│   ├── main.cpp       // Entry point of the application
│   ├── canvas.cpp     // Implementation of the Canvas class
│   ├── shapes.cpp     // Implementation of shape classes
│   └── utils.cpp      // Implementation of utility functions
├── tests
│   ├── test_canvas.cpp // Unit tests for the Canvas class
│   └── test_shapes.cpp // Unit tests for shape classes
├── CMakeLists.txt     // CMake configuration file
└── README.md          // Project documentation
```

## Usage

1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Build the project using CMake:
   ```
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Run the application:
   ```
   ./miniCad
   ```

## Testing

To run the tests, ensure you have built the project and then execute the test binaries located in the `tests` directory.

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue for any suggestions or improvements.