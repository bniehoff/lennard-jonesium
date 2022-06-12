# Lennard-Jones Fluid Simulation

In this folder, we run various example simulations of the Lennard-Jones system, and collect and interpret the results.  We will begin discussion here, and then provide links to individual Jupyter notebooks with further details on the results.

## Introduction

The Lennard-Jones fluid is a toy model meant to approximate systems of neutral atoms (in particular Argon) near their solid-fluid phase transition (we do not speculate whether the fluid is a liquid or a gas).  The first assumption of the model is that the individual fluid particles (i.e. Argon atoms) have no internal degrees of freedom (including no net dipole moment).  At the same time, however, the particles are assumed to be polarizable, which gives rise to a weak electromagnetic attractive potential with $1/r^6$ falloff.  Futhermore, it is assumed that the fluid particles have some internal structure which produces a "hard wall" repulsive force, whose potential is taken to scale with $1/r^{12}$.  This gives the "6-12" Lennard-Jones potential:

$$
\begin{equation}
U_{ij} = 4 \epsilon \bigg[ \bigg( \frac{\sigma}{r_{ij}} \bigg)^{12} - \bigg( \frac{\sigma}{r_{ij}} \bigg)^{6} \bigg].
\end{equation}
$$

The assumptions that we have just described appear contradictory: the particles have no internal degrees of freedom, and yet they have internal structure which allows them to be polarized, and which produces the repulsive part of the potential.  This contradiction is resolved by stipulating that this approximation hold only in the low-energy regime, where the internal degrees of freedom are not excited, and thus do not contribute to the thermodynamics.

### Truncating the potential

The potential energy function above is the one we theoretically want to model; however, it has a flaw in that it has infinite range.  In order to simulate a pairwise force efficiently, we need it to have a limited range, so that we need only compute the forces between atoms which are in a limited volume (rather than pairwise forces between *all* atoms).  Fortunately, the $1/r^6$ falloff of the Lennard-Jones potential is quite fast, so we can in principle impose a cutoff at some cutoff distance $r_c$.  One can show that already around

$$
r_c \approx 2.5 \, \sigma
$$

the Lennard-Jones potential is quite close to zero.

However, this is not the complete story.  Of course the interparticle force is given by the gradient of the potential,

$$
\vec F_{ij} = - \vec \nabla U_{ij},
$$

so we would not want to have a hard cutoff at $r = r_c$, since this would introduce a jump in the potential, and thus an infinite force.  Instead we cutoff the potential *smoothly*, by introducing a quadratic spline function of the form

$$
\begin{equation}
S_{ij} = \alpha + \beta \bigg[ \bigg( \frac{r_{ij}}{r_c} \bigg)^2 - 1 \bigg],
\end{equation}
$$

and modifying the potential function as follows:

$$
\begin{equation}
\hat U_{ij} = U_{ij} + S_{ij} = 4 \epsilon \bigg[ \bigg( \frac{\sigma}{r_{ij}} \bigg)^{12} - \bigg( \frac{\sigma}{r_{ij}} \bigg)^{6} \bigg] + \alpha + \beta \bigg[ \bigg( \frac{r_{ij}}{r_c} \bigg)^2 - 1 \bigg].
\end{equation}
$$

To determine the spline parameters $\alpha, \beta$, we impose that the potential $\hat U_{ij}$ and its first derivative should vanish at $r_{ij} = r_c$.  This obtains

$$
\begin{equation}
\alpha = -4 \epsilon \bigg[ \bigg( \frac{\sigma}{r_c} \bigg)^{12} - \bigg( \frac{\sigma}{r_c} \bigg)^{6} \bigg], \qquad \beta = 12 \epsilon \bigg[ 2 \bigg( \frac{\sigma}{r_c} \bigg)^{12} - \bigg( \frac{\sigma}{r_c} \bigg)^{6} \bigg].
\end{equation}
$$

In this way, the modified potential smoothly falls off to zero at the cutoff distance.

Note that we could have just as well used a *linear* counterterm rather than quadratic.  The advantage of using the quadratic one is that the final potential contains only even powers of $r_{ij}$, and thus can be computed directly from the *square* distance, without having to take a costly square root.

### Units

The internal simulation runs using dimensionless units, which allows the results to be interpreted for a variety of real-world substances via rescaling.  For Argon in particular, the units are normalized by the following dimensionful quantities:

$$
\begin{align}
&\text{Length} : \sigma = 3.405 \times 10^{-10} ~ \mathrm{m}, & &\text{Time} : \tau = \sqrt{\frac{m_\text{Ar} \sigma^2}{\epsilon}} = 2.15 \times 10^{-12} ~ \mathrm{s}, \\
&\text{Energy} : \epsilon = 1.65 \times 10^{-21} ~ \mathrm{J}, & &\text{Temperature} : \frac{\epsilon}{k_\text{B}} = 120 ~ \mathrm{K}, \\
&\text{Mass} : m_\text{Ar} = 6.63 \times 10^{-26} ~ \mathrm{kg}, & &\text{Entropy} : k_\text{B} = 1.38 \times 10^{-23} ~ \mathrm{J} / \mathrm{K},
\end{align}
$$

Here $m_\text{Ar}$ is the atomic mass of Argon, and $k_\text{B}$ is Boltzmann's constant.  The simulations use a time step of $0.005 \tau$, so that 200 time steps make up one unit of time.

In sweep simulations, we will investigate a range of temperatures from $0.1$ to $2.0$ and densities from $0.1$ to $1.4$.  This corresponds to a temperature range of $12 ~ \mathrm{K} = -261 ~ {}^\circ \mathrm{C}$ to $240 ~ \mathrm{K} = -33 ~ {}^\circ \mathrm{C}$, and a density range of $2.5$ to $35$ particles per cubic nanometer.

## Simulations

In this folder, we run the following simulations, with further information in the notebooks at these links:

- [Single run demo](quick_single_run/simple_run.pdf) (500 particles)
- [Sweep run demo](quick_sweep_run/small_system.pdf) (500 particles)
