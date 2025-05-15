<h1 align="center">service template for <code>c</code></h1>
<div align="center">
  <a href="https://github.com/VU-ASE/service-template-c/releases/latest">Latest release</a>
  <span>&nbsp;&nbsp;•&nbsp;&nbsp;</span>
  <a href="https://ase.vu.nl/docs/framework/glossary/service">About a service</a>
  <span>&nbsp;&nbsp;•&nbsp;&nbsp;</span>
  <a href="https://ase.vu.nl/docs/framework/glossary/roverlib">About the roverlib</a>
  <br />
</div>
<br/>

**When building a service that runs on the Rover and should interface the ASE framework, you will most likely want to use a [roverlib](https://ase.vu.nl/docs/framework/glossary/roverlib). This is a C template that incorporates [`roverlib-c`](https://github.com/VU-ASE/roverlib-c), meant to run on the Rover.**

## Initialize a C service

Instead of cloning this repository, it is recommended to initialize this C service using `roverctl` as follows:

```bash
roverctl service init c --name c-example --source github.com/author/c-example
```

Read more about using `roverctl` to initialize services [here](https://ase.vu.nl/docs/framework/Software/rover/roverctl/usage#initialize-a-service).


