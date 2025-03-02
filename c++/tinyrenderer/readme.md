
## Compile
```mingw32-make```



## Remark

### Line5($x_0$, $y_0$, $x_1$, $y_1$, Color)
 ![image](./public/Bresenham’s%20Line%20Drawing%20Algorithm.png)



### Barycentric-coordinates
```math
\begin{align}
    px = w_A * ax + w_B * bx + w_C * cx \\
    py = w_A * ay + w_B * by + w_C * cy \\
    1  = w_A + w_B + w_C\\
\end{align}
```

```math
\begin{pmatrix}
p_x \\
p_y \\
1
\end{pmatrix}
=
\begin{pmatrix}
a_x & b_x & c_x \\
a_y & b_y & c_y \\
1 & 1 & 1
\end{pmatrix}
\begin{pmatrix}
w_A \\
w_B \\
w_C
\end{pmatrix}
```

### Projection matrix

```math
\frac{d}{z}
\begin{pmatrix}
1 & 0   & 0   & 0\\
0   & 1 & 0   & 0\\
0   & 0   & 1 & 0\\
0   & 0   & \frac{z}{d}   & 0
\end{pmatrix}
\begin{pmatrix}
x\\
y\\
z\\
z/d  
\end{pmatrix}
=\begin{pmatrix}
x\times\frac{d}{z}\\
y\times\frac{d}{z}\\
z\times\frac{d}{z}\\
1 
\end{pmatrix}
=\begin{pmatrix}
x_p\\
y_P\\
z_P\\
1
\end{pmatrix}
```
### Convert to normalized device coordinates ( NDC )
```math
\begin{align}
y_{ndc} &= (y_p-\text{bot})\times\frac{2}{\text{top}-\text{bot}}+-1\\
y_{ndc} &= \frac{2\times{y_p}}{\text{top}-\text{bot}}-\frac{2\times{\text{bot}}}{\text{top}-\text{bot}}- \frac{\text{top}-\text{bot}}{\text{top}-\text{bot}}\\
y_{ndc} &=\frac{2\times{y_p}}{\text{top}-\text{bot}}- \frac{2\times\text{bot}+\text{top}-\text{bot}}{\text{top}-\text{bot}}\\
y_{ndc} &=\frac{2\times{y_p}}{\text{top}-\text{bot}}- \frac{\text{top}+\text{bot}}{\text{top}-\text{bot}}\\
\end{align}
```

The view point is in center and symmetric
![side view](./asset/sideViewNDC.png)
```math
\begin{align}
y_{ndc} &= y_p\times\frac{2}{\text{top}-\text{bot}} +0 \\
\end{align}
```
The view point is not in center and not symmetric e.g $(\text{top length} \not = \text{bottom length})$
![side view](./asset/asySideViewNDC.png)
```math
\begin{align}
y_{ndc} &=\frac{2\times{y_p}}{\text{top}-\text{bot}}- \frac{\text{top}+\text{bot}}{\text{top}-\text{bot}}\\
\end{align}
```

Replace $y_p$ with $\frac{y_\text{cam} \times -\text{near}}{z_\text{cam}}$ where $z_\text{cam}$ <0

```math
\begin{split}
y_{ndc} &=\frac{2\times \frac{y_\text{cam}\times -\text{near}}{z_\text{cam}}}{\text{top}-\text{bot}}- \frac{\text{top}+\text{bot}}{\text{top}-\text{bot}}\\
y_{ndc} &=\frac{2\times y_\text{cam} \times \text{near}}{-z_\text{cam} \times (\text{top}-\text{bot})}- \frac{\text{top}+\text{bot}}{\text{top}-\text{bot}}\\
y_{ndc} &=\frac{\frac{2\times y_\text{cam}}{ \text{top}-\text{bot}}\times \text{near}}{-z_\text{cam} }- \frac{\text{top}+\text{bot}}{\text{top}-\text{bot}}\\
y_{ndc} &=\frac{\frac{2\times \text{near}}{ \text{top}-\text{bot}}}{-z_\text{cam} }y_\text{cam}- \frac{-z_\text{cam}\times (\text{top}+\text{bot})}{-z_\text{cam} \times (\text{top}-\text{bot})}\\
y_{ndc} &=\frac{\frac{2\times \text{near}}{ \text{top}-\text{bot}}}{-z_\text{cam} }y_\text{cam} + \frac{\frac{\text{top}+\text{bot}}{\text{top}-\text{bot}}}{-z_\text{cam}}z_\text{cam}\\
y_{ndc} &=\frac{{\frac{2 \times \text{near}}{ \text{top}-\text{bot}}}y_\text{cam} + \frac{\text{top}+\text{bot}}{\text{top}-\text{bot}}z_\text{cam}}{-z_\text{cam}}
\end{split}
```

$x_\text{ndc}$:
```math
\begin{align}
x_{ndc} &=\frac{{\frac{2\times \text{near}}{ \text{right}-\text{left}}}x_\text{cam} + \frac{\text{right}+\text{left}}{\text{right}-\text{left}}z_\text{cam}}{-z_\text{cam}}\\
\end{align}
```

$z_\text{ndc}$:

(-near, -far) map (-1, 1)

```math
\begin{split}
z_{ndc} &= \frac{Az+B}{-z} 
\begin{cases}
-1 &= \frac{A(-near)+B}{near} \text{ z = -near}\\
1 &= \frac{A(-far)+B}{far} \text{ z = -far}
\end{cases} 
\end{split}
```
```math
\begin{align}
-\text{near} = A(-\text{near})+B\\
\text{far} = A(-\text{far})+B
\end{align}
```

```math
\begin{split}

\text{far} + \text{near} &= A(-\text{far})+ B - A(-\text{near})- B \\
\text{far} + \text{near} &= A(-\text{far} + \text{near}) \\
A &= - \frac{\text{far}+\text{near}} {\text{far}-\text{near}}
\end{split}
```
```math
\begin{split}
\text{far} &= -\frac{\text{far}+\text{near}} {\text{far}-\text{near}}(-\text{far})+B\\
\text{far} &= \frac{\text{far}+\text{near}} {\text{far}-\text{near}}(\text{far})+B\\
\text{far}(1-\frac{\text{far}+\text{near}} {\text{far}-\text{near}}) &=B\\
\text{far}(\frac{\text{far}-\text{near}} {\text{far}-\text{near}}-\frac{\text{far}+\text{near}} {\text{far}-\text{near}}) &= B\\
B &= \text{far}(-\frac{2\text{near}} {\text{far}-\text{near}})\\
B &= -\frac{2\text{far} \times \text{near}} {\text{far}-\text{near}}
\end{split}
```
```math
\begin{split}
z_{ndc} &= \frac{-\frac{\text{far}+\text{near}} {\text{far}-\text{near}}z-\frac{2\text{far} \times \text{near}} {\text{far}-\text{near}}}{-z}\\
\end{split}
```

```math
\begin{pmatrix}
{\frac{2\times \text{near}}{ \text{right}-\text{left}}} & 0   &\frac{\text{right}+\text{left}}{\text{right}-\text{left}}  & 0\\
0   & \frac{2\times \text{near}}{ \text{top}-\text{bot}} & \frac{\text{top}+\text{bot}}{\text{top}-\text{bot}}   & 0\\
0   & 0   & -\frac{\text{far}+\text{near}} {\text{far}-\text{near}} & -\frac{2\text{far} \times \text{near}} {\text{far}-\text{near}}\\
0   & 0   & -1   & 0
\end{pmatrix}
\begin{pmatrix}
x_\text{cam}\\
y_\text{cam}\\
z_\text{cam}\\
1
\end{pmatrix}
=\begin{pmatrix}
x'\\
y'\\
z'\\
-z
\end{pmatrix}
->\begin{pmatrix}
x_\text{ndc}\\
y_\text{ndc}\\
z_\text{ndc}\\
1
\end{pmatrix}
```

### To Camera Space
change of basis: https://www.3blue1brown.com/lessons/change-of-basis
```math
M^{-1}=
\frac{1}{det(\text{M})}
\begin{pmatrix}
x_1 & y_1   &z_1  &0\\
x_2 & y_2   &z_2  &0\\
x_3 & y_3   &z_3  &0\\
0   & 0     &0    &1
\end{pmatrix}^T
```
where det(M) == 1
```math
ModelView= M^{-1}T^{-1}=
\begin{pmatrix}
x_1 & x_2   &x_3  &0\\
y_1 & y_2   &y_3  &0\\
z_1 & z_2   &z_3  &0\\
0   & 0     &0    &1
\end{pmatrix}
\begin{pmatrix}
1   & 0     &0  &-x_c\\
0   & 1     &0  &-y_c\\
0   & 0     &1  &-z_c\\
0   & 0     &0    &1
\end{pmatrix}
```
($x_c,y_c,z_c$) camera position


## Reference
1. https://github.com/ssloy/tinyrenderer 
2. https://www.songho.ca/opengl/gl_projectionmatrix.html